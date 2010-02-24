/*
The file contains functions and vars used for the deferred shading/post-processing stage/SSAO pass.
*/

#include "renderer.h"
#include "Resource.h"
#include "Texture.h"
#include "Scene.h"
#include "r_private.h"
#include "fbo.h"
#include "Camera.h"

namespace r {
namespace pps {
namespace ssao {


/*
=======================================================================================================================================
VARS                                                                                                                                  =
=======================================================================================================================================
*/
static fbo_t fbo, blur_fbo; // yet another FBO

float rendering_quality = 0.25; // the rendering_quality of the SSAO fai. Chose low so it can blend
bool enabled = true;

static uint wwidth, wheight; // window width and height

Texture fai, blured_fai; // SSAO factor

static ShaderProg* shdr_ppp_ssao, * blur_shdr;

static Texture* noise_map;


//=====================================================================================================================================
// InitBlurFBO                                                                                                                        =
//=====================================================================================================================================
static void InitBlurFBO()
{
	// create FBO
	blur_fbo.Create();
	blur_fbo.Bind();

	// inform in what buffers we draw
	blur_fbo.SetNumOfColorAttachements(1);

	// create the texes
	blured_fai.createEmpty2D( wwidth, wheight, GL_ALPHA8, GL_ALPHA );
	blured_fai.texParameter( GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	blured_fai.texParameter( GL_TEXTURE_MIN_FILTER, GL_NEAREST );

	// attach
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, blured_fai.getGlId(), 0 );

	// test if success
	if( !blur_fbo.IsGood() )
		FATAL( "Cannot create deferred shading post-processing stage SSAO blur FBO" );

	// unbind
	blur_fbo.Unbind();
}


/*
=======================================================================================================================================
init                                                                                                                                  =
=======================================================================================================================================
*/
void Init()
{
	if( rendering_quality<0.0 || rendering_quality>1.0 ) ERROR("Incorect r::pps:ssao::rendering_quality");
	wwidth = r::pps::ssao::rendering_quality * r::w;
	wheight = r::pps::ssao::rendering_quality * r::h;

	// create FBO
	fbo.Create();
	fbo.Bind();

	// inform in what buffers we draw
	fbo.SetNumOfColorAttachements(1);

	// create the texes
	fai.createEmpty2D( wwidth, wheight, GL_ALPHA8, GL_ALPHA );
	fai.texParameter( GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	fai.texParameter( GL_TEXTURE_MIN_FILTER, GL_NEAREST );

	// attach
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, fai.getGlId(), 0 );

	// test if success
	if( !fbo.IsGood() )
		FATAL( "Cannot create deferred shading post-processing stage SSAO pass FBO" );

	// unbind
	fbo.Unbind();


	// init shaders
	shdr_ppp_ssao = rsrc::shaders.load( "shaders/pps_ssao.glsl" );

	// load noise map and disable temporaly the texture compression and enable mipmaping
	bool tex_compr = r::texture_compression;
	bool mipmaping = r::mipmaping;
	r::texture_compression = false;
	r::mipmaping = true;
	noise_map = rsrc::textures.load( "gfx/noise3.tga" );
	noise_map->texParameter( GL_TEXTURE_WRAP_S, GL_REPEAT );
	noise_map->texParameter( GL_TEXTURE_WRAP_T, GL_REPEAT );
	//noise_map->texParameter( GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	//noise_map->texParameter( GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	r::texture_compression = tex_compr;
	r::mipmaping = mipmaping;

	// blur FBO
	InitBlurFBO();
	blur_shdr = rsrc::shaders.load( "shaders/pps_ssao_blur.glsl" );
}


/*
=======================================================================================================================================
RunPass                                                                                                                               =
=======================================================================================================================================
*/
void RunPass( const Camera& cam )
{
	fbo.Bind();

	r::SetViewport( 0, 0, wwidth, wheight );

	glDisable( GL_BLEND );
	glDisable( GL_DEPTH_TEST );

	// fill SSAO FAI
	shdr_ppp_ssao->bind();
	glUniform2fv( shdr_ppp_ssao->GetUniLoc(0), 1, &(vec2_t(cam.getZNear(), cam.getZFar()))[0] );
	shdr_ppp_ssao->locTexUnit( shdr_ppp_ssao->GetUniLoc(1), ms::depth_fai, 0 );
	shdr_ppp_ssao->locTexUnit( shdr_ppp_ssao->GetUniLoc(2), *noise_map, 1 );
	shdr_ppp_ssao->locTexUnit( shdr_ppp_ssao->GetUniLoc(3), ms::normal_fai, 2 );
	r::DrawQuad( shdr_ppp_ssao->getAttribLoc(0) ); // Draw quad


	// second pass. blur
	blur_fbo.Bind();
	blur_shdr->bind();
	blur_shdr->locTexUnit( blur_shdr->GetUniLoc(0), fai, 0 );
	r::DrawQuad( blur_shdr->getAttribLoc(0) ); // Draw quad

	// end
	fbo_t::Unbind();
}


}}} // end namespaces
