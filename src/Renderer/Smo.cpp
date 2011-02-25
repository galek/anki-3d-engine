#include "Smo.h"
#include "Renderer.h"
#include "Light.h"
#include "LightRsrc.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Camera.h"
#include "Vao.h"
#include "Vbo.h"


//======================================================================================================================
// Statics                                                                                                             =
//======================================================================================================================

float Smo::sMOUvSCoords[] = {-0.000000, 0.000000, -1.000000, 0.500000, 0.500000, -0.707107, 0.707107, 0.000000, -0.707107, 0.500000, 0.500000, 0.707107, 0.000000, 0.000000, 1.000000, 0.707107, 0.000000, 0.707107, -0.000000, 0.707107, 0.707107, 0.000000, 0.000000, 1.000000, 0.500000, 0.500000, 0.707107, -0.000000, 0.000000, -1.000000, -0.000000, 0.707107, -0.707107, 0.500000, 0.500000, -0.707107, -0.000000, 0.000000, -1.000000, -0.500000, 0.500000, -0.707107, -0.000000, 0.707107, -0.707107, -0.500000, 0.500000, 0.707107, 0.000000, 0.000000, 1.000000, -0.000000, 0.707107, 0.707107, -0.707107, -0.000000, 0.707107, 0.000000, 0.000000, 1.000000, -0.500000, 0.500000, 0.707107, -0.000000, 0.000000, -1.000000, -0.707107, -0.000000, -0.707107, -0.500000, 0.500000, -0.707107, -0.000000, 0.000000, -1.000000, -0.500000, -0.500000, -0.707107, -0.707107, -0.000000, -0.707107, -0.500000, -0.500000, 0.707107, 0.000000, 0.000000, 1.000000, -0.707107, -0.000000, 0.707107, 0.000000, -0.707107, 0.707107, 0.000000, 0.000000, 1.000000, -0.500000, -0.500000, 0.707107, -0.000000, 0.000000, -1.000000, 0.000000, -0.707107, -0.707107, -0.500000, -0.500000, -0.707107, -0.000000, 0.000000, -1.000000, 0.500000, -0.500000, -0.707107, 0.000000, -0.707107, -0.707107, 0.500000, -0.500000, 0.707107, 0.000000, 0.000000, 1.000000, 0.000000, -0.707107, 0.707107, 0.707107, 0.000000, 0.707107, 0.000000, 0.000000, 1.000000, 0.500000, -0.500000, 0.707107, -0.000000, 0.000000, -1.000000, 0.707107, 0.000000, -0.707107, 0.500000, -0.500000, -0.707107, 0.500000, -0.500000, -0.707107, 0.707107, 0.000000, -0.707107, 1.000000, 0.000000, -0.000000, 0.500000, -0.500000, -0.707107, 1.000000, 0.000000, -0.000000, 0.707107, -0.707107, 0.000000, 0.707107, -0.707107, 0.000000, 1.000000, 0.000000, -0.000000, 0.707107, 0.000000, 0.707107, 0.707107, -0.707107, 0.000000, 0.707107, 0.000000, 0.707107, 0.500000, -0.500000, 0.707107, 0.000000, -1.000000, 0.000000, 0.707107, -0.707107, 0.000000, 0.500000, -0.500000, 0.707107, 0.000000, -1.000000, 0.000000, 0.500000, -0.500000, 0.707107, 0.000000, -0.707107, 0.707107, 0.000000, -0.707107, -0.707107, 0.500000, -0.500000, -0.707107, 0.707107, -0.707107, 0.000000, 0.000000, -0.707107, -0.707107, 0.707107, -0.707107, 0.000000, 0.000000, -1.000000, 0.000000, -0.500000, -0.500000, -0.707107, 0.000000, -0.707107, -0.707107, -0.707107, -0.707107, 0.000000, 0.000000, -0.707107, -0.707107, 0.000000, -1.000000, 0.000000, -0.707107, -0.707107, 0.000000, -0.707107, -0.707107, 0.000000, 0.000000, -1.000000, 0.000000, 0.000000, -0.707107, 0.707107, -0.707107, -0.707107, 0.000000, 0.000000, -0.707107, 0.707107, -0.500000, -0.500000, 0.707107, -1.000000, -0.000000, 0.000000, -0.707107, -0.707107, 0.000000, -0.500000, -0.500000, 0.707107, -1.000000, -0.000000, 0.000000, -0.500000, -0.500000, 0.707107, -0.707107, -0.000000, 0.707107, -0.707107, -0.000000, -0.707107, -0.500000, -0.500000, -0.707107, -0.707107, -0.707107, 0.000000, -0.707107, -0.000000, -0.707107, -0.707107, -0.707107, 0.000000, -1.000000, -0.000000, 0.000000, -0.500000, 0.500000, -0.707107, -0.707107, -0.000000, -0.707107, -1.000000, -0.000000, 0.000000, -0.500000, 0.500000, -0.707107, -1.000000, -0.000000, 0.000000, -0.707107, 0.707107, 0.000000, -0.707107, 0.707107, 0.000000, -1.000000, -0.000000, 0.000000, -0.707107, -0.000000, 0.707107, -0.707107, 0.707107, 0.000000, -0.707107, -0.000000, 0.707107, -0.500000, 0.500000, 0.707107, -0.000000, 1.000000, 0.000000, -0.707107, 0.707107, 0.000000, -0.500000, 0.500000, 0.707107, -0.000000, 1.000000, 0.000000, -0.500000, 0.500000, 0.707107, -0.000000, 0.707107, 0.707107, -0.000000, 0.707107, -0.707107, -0.500000, 0.500000, -0.707107, -0.707107, 0.707107, 0.000000, -0.000000, 0.707107, -0.707107, -0.707107, 0.707107, 0.000000, -0.000000, 1.000000, 0.000000, 0.500000, 0.500000, -0.707107, -0.000000, 0.707107, -0.707107, -0.000000, 1.000000, 0.000000, 0.500000, 0.500000, -0.707107, -0.000000, 1.000000, 0.000000, 0.707107, 0.707107, 0.000000, 0.707107, 0.707107, 0.000000, -0.000000, 1.000000, 0.000000, -0.000000, 0.707107, 0.707107, 0.707107, 0.707107, 0.000000, -0.000000, 0.707107, 0.707107, 0.500000, 0.500000, 0.707107, 1.000000, 0.000000, -0.000000, 0.707107, 0.707107, 0.000000, 0.500000, 0.500000, 0.707107, 1.000000, 0.000000, -0.000000, 0.500000, 0.500000, 0.707107, 0.707107, 0.000000, 0.707107, 0.707107, 0.000000, -0.707107, 0.500000, 0.500000, -0.707107, 0.707107, 0.707107, 0.000000, 0.707107, 0.000000, -0.707107, 0.707107, 0.707107, 0.000000, 1.000000, 0.000000, -0.000000};

float Smo::camPositions[] = {
	0.0, 0.0, 0.0, // Eye
	1.0, 1.0, -1.0, // Top right
	-1.0, 1.0, -1.0, // Top left
	-1.0, -1.0, -1.0, // Bottom left
	1.0, -1.0, -1.0 // Bottom right
};

//======================================================================================================================
// init                                                                                                                =
//======================================================================================================================
void Smo::init(const RendererInitializer& /*initializer*/)
{
	sProg.loadRsrc("shaders/IsSmo.glsl");

	//
	// Geometry stuff
	//

	// Sphere
	//
	spherePositionsVbo.create(GL_ARRAY_BUFFER, sizeof(sMOUvSCoords), sMOUvSCoords, GL_STATIC_DRAW);
	sphereVao.create();
	sphereVao.attachArrayBufferVbo(spherePositionsVbo, *sProg->findAttribVar("position"), 3, GL_FLOAT, false, 0, NULL);

	// Camera
	//

	// 4 vertex positions: eye, top-right, top-left, bottom-left, bottom-right
	cameraPositionsVbo.create(GL_ARRAY_BUFFER, sizeof(float) * 3 * 5, camPositions, GL_STATIC_DRAW);

	// The vert indeces
	enum {EYE, TR, TL, BL, BR}; // Vert positions

	ushort vertIndeces[6][3] = {
		{EYE, BR, TR}, // Right triangle
		{EYE, TR, TL}, // Top
		{EYE, TL, BL}, // Left
		{EYE, BL, BR}, // Bottom
		{BR, BL, TL}, {TL, TR, BR} // Front
	};

	cameraVertIndecesVbo.create(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertIndeces), vertIndeces, GL_STATIC_DRAW);

	cameraVao.create();
	cameraVao.attachArrayBufferVbo(cameraPositionsVbo, *sProg->findAttribVar("position"), 3, GL_FLOAT, false, 0, NULL);
	cameraVao.attachElementArrayBufferVbo(cameraVertIndecesVbo);
}


//======================================================================================================================
// run [PointLight]                                                                                                    =
//======================================================================================================================
void Smo::run(const PointLight& light)
{
	// set GL
	glStencilFunc(GL_ALWAYS, 0x1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glColorMask(false, false, false, false);
	glClear(GL_STENCIL_BUFFER_BIT);

	glDisable(GL_CULL_FACE);

	// set shared prog
	const float scale = 1.2; // we scale the sphere a little
	sProg->bind();
	Mat4 modelMat = Mat4(light.getWorldTransform().getOrigin(), Mat3::getIdentity(), light.getRadius() * scale);
	Mat4 trf = r.getViewProjectionMat() * modelMat;
	sProg->findUniVar("modelViewProjectionMat")->setMat4(&trf);

	// render sphere to the stencil buffer
	sphereVao.bind();
	glDrawArrays(GL_TRIANGLES, 0, sizeof(sMOUvSCoords) / sizeof(float) / 3);
	sphereVao.unbind();

	// restore GL
	glEnable(GL_CULL_FACE);
	glColorMask(true, true, true, true);

	glStencilFunc(GL_EQUAL, 0x1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}


//======================================================================================================================
// run [SpotLight]                                                                                                     =
//======================================================================================================================
void Smo::run(const SpotLight& light)
{
	const Camera& lcam = light.getCamera();

	// set GL state
	glStencilFunc(GL_ALWAYS, 0x1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glClear(GL_STENCIL_BUFFER_BIT);

	glColorMask(false, false, false, false);
	glDisable(GL_CULL_FACE);

	// Calc the camera shape scale matrix
	Mat4 scaleMat(Mat4::getIdentity());
	scaleMat(0, 0) = tan(lcam.getFovX() / 2.0) * lcam.getZFar(); // Scale in x
	scaleMat(1, 1) = tan(lcam.getFovY() / 2.0) * lcam.getZFar(); // Scale in y
	scaleMat(2, 2) = lcam.getZFar(); // Scale in z

	sProg->bind();
	Mat4 modelMat = Mat4(lcam.getWorldTransform());

	Mat4 trf = r.getViewProjectionMat() * modelMat * scaleMat;
	sProg->findUniVar("modelViewProjectionMat")->setMat4(&trf);

	//
	// Render
	//
	cameraVao.bind();
	glDrawElements(GL_TRIANGLES, 6 * 3, GL_UNSIGNED_SHORT, 0);
	cameraVao.unbind();

	// restore GL state
	glEnable(GL_CULL_FACE);
	glColorMask(true, true, true, true);

	glStencilFunc(GL_EQUAL, 0x1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}
