#include "Renderer.h"
#include "Camera.h" /// @todo remove this
#include "RendererInitializer.h"
#include "Material.h"


//======================================================================================================================
// Vars                                                                                                                =
//======================================================================================================================
float Renderer::quadVertCoords [][2] = { {1.0,1.0}, {0.0,1.0}, {0.0,0.0}, {1.0,0.0} };


//======================================================================================================================
// Constructor                                                                                                         =
//======================================================================================================================
Renderer::Renderer():
	width( 640 ),
	height( 480 ),
	ms( *this ),
	is( *this ),
	pps( *this ),
	dbg( *this )
{
}

//======================================================================================================================
// init                                                                                                                =
//======================================================================================================================
void Renderer::init( const RendererInitializer& initializer )
{
	// set from the initializer
	ms.ez.enabled = initializer.ms.ez.enabled;
	is.sm.enabled = initializer.is.sm.enabled;
	is.sm.pcfEnabled = initializer.is.sm.pcfEnabled;
	is.sm.bilinearEnabled = initializer.is.sm.bilinearEnabled;
	is.sm.resolution = initializer.is.sm.resolution;
	pps.hdr.enabled = initializer.pps.hdr.enabled;
	pps.hdr.renderingQuality = initializer.pps.hdr.renderingQuality;
	pps.ssao.enabled = initializer.pps.ssao.enabled;
	pps.ssao.renderingQuality = initializer.pps.ssao.renderingQuality;
	pps.ssao.bluringQuality = initializer.pps.ssao.bluringQuality;
	dbg.enabled = initializer.dbg.enabled;
	width = initializer.width;
	height = initializer.height;

	aspectRatio = float(width)/height;

	// a few sanity checks
	if( width < 10 || height < 10 )
	{
		FATAL( "Incorrect width" );
	}

	// init the stages. Careful with the order!!!!!!!!!!
	ms.init();
	is.init();
	pps.init();
	dbg.init();
}


//======================================================================================================================
// render                                                                                                              =
//======================================================================================================================
void Renderer::render( Camera& cam_ )
{
	cam = &cam_;

	ms.run();
	is.run();
	pps.run();
	//dbg.run();

	++framesNum;
}


//======================================================================================================================
// drawQuad                                                                                                            =
//======================================================================================================================
void Renderer::drawQuad( int vertCoordsUniLoc )
{
	DEBUG_ERR( vertCoordsUniLoc == -1 );
	glVertexAttribPointer( vertCoordsUniLoc, 2, GL_FLOAT, false, 0, quadVertCoords );
	glEnableVertexAttribArray( vertCoordsUniLoc );
	glDrawArrays( GL_QUADS, 0, 4 );
	glDisableVertexAttribArray( vertCoordsUniLoc );
}


//======================================================================================================================
// setupMaterial                                                                                                       =
//======================================================================================================================
void Renderer::setupMaterial( const Material& mtl, const SceneNode& sceneNode, const Camera& cam )
{
	mtl.shaderProg->bind();
	uint textureUnit = 0;

	//
	// FFP stuff
	//
	if( mtl.blends )
	{
		glEnable( GL_BLEND );
		//glDisable( GL_BLEND );
		glBlendFunc( mtl.blendingSfactor, mtl.blendingDfactor );
	}
	else
		glDisable( GL_BLEND );


	if( mtl.depthTesting )
		glEnable( GL_DEPTH_TEST );
	else
		glDisable( GL_DEPTH_TEST );

	if( mtl.wireframe )
		glPolygonMode( GL_FRONT, GL_LINE );
	else
		glPolygonMode( GL_FRONT, GL_FILL );


	//
	// matrices
	//
	Mat4 modelMat( sceneNode.getWorldTransform() );
	const Mat4& projectionMat = cam.getProjectionMatrix();
	const Mat4& viewMat = cam.getViewMatrix();
	Mat4 modelViewMat;
	Mat3 normalMat;
	Mat4 modelViewProjectionMat;

	// should I calculate the modelViewMat ?
	if( mtl.stdUniVars[ Material::SUV_MODELVIEW_MAT ] ||
			mtl.stdUniVars[ Material::SUV_MODELVIEWPROJECTION_MAT ] ||
			mtl.stdUniVars[ Material::SUV_NORMAL_MAT ] )
	{
		modelViewMat = Mat4::combineTransformations( viewMat, modelMat );
	}

	// set all the matrices
	if( mtl.stdUniVars[ Material::SUV_MODEL_MAT ] )
		mtl.stdUniVars[ Material::SUV_MODEL_MAT ]->setMat4( &modelMat );

	if( mtl.stdUniVars[ Material::SUV_VIEW_MAT ] )
		mtl.stdUniVars[ Material::SUV_VIEW_MAT ]->setMat4( &viewMat );

	if( mtl.stdUniVars[ Material::SUV_PROJECTION_MAT ] )
		mtl.stdUniVars[ Material::SUV_PROJECTION_MAT ]->setMat4( &projectionMat );

	if( mtl.stdUniVars[ Material::SUV_MODELVIEW_MAT ] )
		mtl.stdUniVars[ Material::SUV_MODELVIEW_MAT ]->setMat4( &modelViewMat );

	if( mtl.stdUniVars[ Material::SUV_NORMAL_MAT ] )
	{
		normalMat = modelViewMat.getRotationPart();
		mtl.stdUniVars[ Material::SUV_NORMAL_MAT ]->setMat3( &normalMat );
	}

	if( mtl.stdUniVars[ Material::SUV_MODELVIEWPROJECTION_MAT ] )
	{
		modelViewProjectionMat = projectionMat * modelViewMat;
		mtl.stdUniVars[ Material::SUV_MODELVIEWPROJECTION_MAT ]->setMat4( &modelViewProjectionMat );
	}


	//
	// FAis
	//
	if( mtl.stdUniVars[ Material::SUV_MS_NORMAL_FAI ] )
		mtl.stdUniVars[ Material::SUV_MS_NORMAL_FAI ]->setTexture( ms.normalFai, textureUnit++ );

	if( mtl.stdUniVars[ Material::SUV_MS_DIFFUSE_FAI ] )
		mtl.stdUniVars[ Material::SUV_MS_DIFFUSE_FAI ]->setTexture( ms.diffuseFai, textureUnit++ );

	if( mtl.stdUniVars[ Material::SUV_MS_SPECULAR_FAI ] )
		mtl.stdUniVars[ Material::SUV_MS_SPECULAR_FAI ]->setTexture( ms.specularFai, textureUnit++ );

	if( mtl.stdUniVars[ Material::SUV_MS_DEPTH_FAI ] )
		mtl.stdUniVars[ Material::SUV_MS_DEPTH_FAI ]->setTexture( ms.depthFai, textureUnit++ );

	if( mtl.stdUniVars[ Material::SUV_IS_FAI ] )
		mtl.stdUniVars[ Material::SUV_IS_FAI ]->setTexture( is.fai, textureUnit++ );

	if( mtl.stdUniVars[ Material::SUV_PPS_FAI ] )
		mtl.stdUniVars[ Material::SUV_PPS_FAI ]->setTexture( pps.fai, textureUnit++ );


	//
	// Other
	//
	if( mtl.stdUniVars[ Material::SUV_RENDERER_SIZE ] )
	{
		Vec2 v( width, height );
		mtl.stdUniVars[ Material::SUV_RENDERER_SIZE ]->setVec2( &v );
	}


	//
	// now loop all the user defined vars and set them
	//
	for( uint i=0; i<mtl.userDefinedVars.size(); i++ )
	{
		const Material::UserDefinedUniVar* udv = &mtl.userDefinedVars[i];
		switch( udv->sProgVar->getGlDataType() )
		{
			// texture
			case GL_SAMPLER_2D:
				udv->sProgVar->setTexture( *udv->value.texture, textureUnit++ );
				break;
			// float
			case GL_FLOAT:
				udv->sProgVar->setFloat( udv->value.float_ );
				break;
			// vec2
			case GL_FLOAT_VEC2:
				udv->sProgVar->setVec2( &udv->value.vec2 );
				break;
			// vec3
			case GL_FLOAT_VEC3:
				udv->sProgVar->setVec3( &udv->value.vec3 );
				break;
			// vec4
			case GL_FLOAT_VEC4:
				udv->sProgVar->setVec4( &udv->value.vec4 );
				break;
		}
	}
}


//======================================================================================================================
// setProjectionMatrix                                                                                                 =
//======================================================================================================================
void Renderer::setProjectionMatrix( const Camera& cam )
{
	glMatrixMode( GL_PROJECTION );
	loadMatrix( cam.getProjectionMatrix() );
}


//======================================================================================================================
// setViewMatrix                                                                                                       =
//======================================================================================================================
void Renderer::setViewMatrix( const Camera& cam )
{
	glMatrixMode( GL_MODELVIEW );
	loadMatrix( cam.getViewMatrix() );
}


//======================================================================================================================
// unproject                                                                                                           =
//======================================================================================================================
Vec3 Renderer::unproject( const Vec3& windowCoords, const Mat4& modelViewMat, const Mat4& projectionMat,
                          const int view[4] )
{
	Mat4 invPm = projectionMat * modelViewMat;
	invPm.invert();

	// the vec is in NDC space meaning: -1<=vec.x<=1 -1<=vec.y<=1 -1<=vec.z<=1
	Vec4 vec;
	vec.x = (2.0*(windowCoords.x-view[0]))/view[2] - 1.0;
	vec.y = (2.0*(windowCoords.y-view[1]))/view[3] - 1.0;
	vec.z = 2.0*windowCoords.z - 1.0;
	vec.w = 1.0;

	Vec4 final = invPm * vec;
	final /= final.w;
	return Vec3( final );
}


//======================================================================================================================
// ortho                                                                                                               =
//======================================================================================================================
Mat4 Renderer::ortho( float left, float right, float bottom, float top, float near, float far )
{
	float difx = right-left;
	float dify = top-bottom;
	float difz = far-near;
	float tx = -(right+left) / difx;
	float ty = -(top+bottom) / dify;
	float tz = -(far+near) / difz;
	Mat4 m;

	m(0,0) = 2.0 / difx;
	m(0,1) = 0.0;
	m(0,2) = 0.0;
	m(0,3) = tx;
	m(1,0) = 0.0;
	m(1,1) = 2.0 / dify;
	m(1,2) = 0.0;
	m(1,3) = ty;
	m(2,0) = 0.0;
	m(2,1) = 0.0;
	m(2,2) = -2.0 / difz;
	m(2,3) = tz;
	m(3,0) = 0.0;
	m(3,1) = 0.0;
	m(3,2) = 0.0;
	m(3,3) = 1.0;

	return m;
}


//======================================================================================================================
// getLastError                                                                                                        =
//======================================================================================================================
const uchar* Renderer::getLastError()
{
	return gluErrorString( glGetError() );
}


//======================================================================================================================
// printLastError                                                                                                      =
//======================================================================================================================
void Renderer::printLastError()
{
	GLenum errid = glGetError();
	if( errid != GL_NO_ERROR )
		ERROR( "OpenGL Error: " << gluErrorString( errid ) );
}
