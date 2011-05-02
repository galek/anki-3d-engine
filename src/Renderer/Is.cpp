#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/array.hpp>

#include "Is.h"
#include "Renderer.h"
#include "Camera.h"
#include "Light.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "LightRsrc.h"
#include "App.h"
#include "LightRsrc.h"
#include "Sm.h"
#include "Smo.h"
#include "Scene.h"


//======================================================================================================================
// Constructor                                                                                                         =
//======================================================================================================================
Is::Is(Renderer& r_):
	RenderingPass(r_),
	sm(r_),
	smo(r_)
{}


//======================================================================================================================
// initFbo                                                                                                             =
//======================================================================================================================
void Is::initFbo()
{
	try
	{
		// create FBO
		fbo.create();
		fbo.bind();

		// init the stencil render buffer
		glGenRenderbuffers(1, &stencilRb);
		glBindRenderbuffer(GL_RENDERBUFFER, stencilRb);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX, r.getWidth(), r.getHeight());
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencilRb);

		// inform in what buffers we draw
		fbo.setNumOfColorAttachements(1);

		// create the FAI
		Renderer::createFai(r.getWidth(), r.getHeight(), GL_RGB, GL_RGB, GL_FLOAT, fai);

		// attach
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fai.getGlId(), 0);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, r.ms.depthFai.getGlId(), 0);

		// test if success
		fbo.checkIfGood();

		// unbind
		fbo.unbind();
	}
	catch(std::exception& e)
	{
		throw EXCEPTION("Cannot create deferred shading illumination stage FBO: " + e.what());
	}
}


//======================================================================================================================
// init                                                                                                                =
//======================================================================================================================
void Is::init(const RendererInitializer& initializer)
{
	// init passes
	smo.init(initializer);
	sm.init(initializer);

	// load the shaders
	ambientPassSProg.loadRsrc("shaders/IsAp.glsl");

	// point light
	pointLightSProg.loadRsrc(ShaderProg::createSrcCodeToCache("shaders/IsLpGeneric.glsl",
	                                                          "#define POINT_LIGHT_ENABLED\n",
	                                                          "Point").c_str());

	// spot light no shadow
	spotLightNoShadowSProg.loadRsrc(ShaderProg::createSrcCodeToCache("shaders/IsLpGeneric.glsl",
	                                                                 "#define SPOT_LIGHT_ENABLED\n",
	                                                                 "Spot_NoShadow").c_str());

	// spot light w/t shadow
	std::string pps = std::string("#define SPOT_LIGHT_ENABLED\n"
	                              "#define SHADOW_ENABLED\n");
	std::string prefix = "Spot_Shadow";
	if(sm.isPcfEnabled())
	{
		pps += "#define PCF_ENABLED\n";
		prefix += "_Pcf";
	}
	spotLightShadowSProg.loadRsrc(ShaderProg::createSrcCodeToCache("shaders/IsLpGeneric.glsl", pps.c_str(),
	                                                               prefix.c_str()).c_str());

	// init the rest
	initFbo();
}


//======================================================================================================================
// ambientPass                                                                                                         =
//======================================================================================================================
void Is::ambientPass(const Vec3& color)
{
	GlStateMachineSingleton::getInstance().setBlendingEnabled(false);

	// set the shader
	ambientPassSProg->bind();

	// set the uniforms
	ambientPassSProg->findUniVar("ambientCol")->set(&color);
	ambientPassSProg->findUniVar("sceneColMap")->set(r.getMs().getDiffuseFai(), 0);

	// Draw quad
	r.drawQuad();
}


//======================================================================================================================
// pointLightPass                                                                                                      =
//======================================================================================================================
void Is::pointLightPass(const PointLight& light)
{
	const Camera& cam = r.getCamera();

	// stencil optimization
	smo.run(light);

	// shader prog
	const ShaderProg& shader = *pointLightSProg; // ensure the const-ness
	shader.bind();

	shader.findUniVar("msNormalFai")->set(r.getMs().getNormalFai(), 0);
	shader.findUniVar("msDiffuseFai")->set(r.getMs().getDiffuseFai(), 1);
	shader.findUniVar("msSpecularFai")->set(r.getMs().getSpecularFai(), 2);
	shader.findUniVar("msDepthFai")->set(r.getMs().getDepthFai(), 3);
	shader.findUniVar("planes")->set(&r.getPlanes());
	shader.findUniVar("limitsOfNearPlane")->set(&r.getLimitsOfNearPlane());
	shader.findUniVar("limitsOfNearPlane2")->set(&r.getLimitsOfNearPlane2());
	float zNear = cam.getZNear();
	shader.findUniVar("zNear")->set(&zNear);
	Vec3 lightPosEyeSpace = light.getWorldTransform().getOrigin().getTransformed(cam.getViewMatrix());
	shader.findUniVar("lightPos")->set(&lightPosEyeSpace);
	shader.findUniVar("lightRadius")->set(&light.getRadius());
	shader.findUniVar("lightDiffuseCol")->set(&light.getDiffuseCol());
	shader.findUniVar("lightSpecularCol")->set(&light.getSpecularCol());

	// render quad
	r.drawQuad();
}


//======================================================================================================================
// spotLightPass                                                                                                       =
//======================================================================================================================
void Is::spotLightPass(const SpotLight& light)
{
	const Camera& cam = r.getCamera();

	// shadow mapping
	if(light.castsShadow() && sm.isEnabled())
	{
		Vec3 zAxis = light.getWorldTransform().getRotation().getColumn(2);
		LineSegment seg(light.getWorldTransform().getOrigin(),
		                -zAxis * light.getCamera().getZFar());

		const Plane& plane = cam.getWSpaceFrustumPlane(Camera::FP_NEAR);

		float dist = seg.testPlane(plane);

		sm.run(light.getCamera(), dist);

		// restore the IS FBO
		fbo.bind();

		// and restore blending and depth test
		GlStateMachineSingleton::getInstance().setBlendingEnabled(true);
		glBlendFunc(GL_ONE, GL_ONE);
		GlStateMachineSingleton::getInstance().setDepthTestEnabled(false);
		Renderer::setViewport(0, 0, r.getWidth(), r.getHeight());
	}

	// stencil optimization
	smo.run(light);

	// set the texture
	//light.getTexture().setRepeat(false);

	// shader prog
	const ShaderProg* shdr;

	if(light.castsShadow() && sm.isEnabled())
	{
		shdr = spotLightShadowSProg.get();
	}
	else
	{
		shdr = spotLightNoShadowSProg.get();
	}

	shdr->bind();

	// bind the FAIs
	shdr->findUniVar("msNormalFai")->set(r.getMs().getNormalFai(), 0);
	shdr->findUniVar("msDiffuseFai")->set(r.getMs().getDiffuseFai(), 1);
	shdr->findUniVar("msSpecularFai")->set(r.getMs().getSpecularFai(), 2);
	shdr->findUniVar("msDepthFai")->set(r.getMs().getDepthFai(), 3);

	// the ???
	shdr->findUniVar("planes")->set(&r.getPlanes());
	shdr->findUniVar("limitsOfNearPlane")->set(&r.getLimitsOfNearPlane());
	shdr->findUniVar("limitsOfNearPlane2")->set(&r.getLimitsOfNearPlane2());
	float zNear = cam.getZNear();
	shdr->findUniVar("zNear")->set(&zNear);

	// the light params
	Vec3 lightPosEyeSpace = light.getWorldTransform().getOrigin().getTransformed(cam.getViewMatrix());
	shdr->findUniVar("lightPos")->set(&lightPosEyeSpace);
	float tmp = light.getDistance();
	shdr->findUniVar("lightRadius")->set(&tmp);
	shdr->findUniVar("lightDiffuseCol")->set(&light.getDiffuseCol());
	shdr->findUniVar("lightSpecularCol")->set(&light.getSpecularCol());
	shdr->findUniVar("lightTex")->set(light.getTexture(), 4);

	// set texture matrix for texture & shadowmap projection
	// Bias * P_light * V_light * inv(V_cam)
	static Mat4 biasMat4(0.5, 0.0, 0.0, 0.5, 0.0, 0.5, 0.0, 0.5, 0.0, 0.0, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0);
	Mat4 texProjectionMat;
	texProjectionMat = biasMat4 * light.getCamera().getProjectionMatrix() *
	                   Mat4::combineTransformations(light.getCamera().getViewMatrix(), Mat4(cam.getWorldTransform()));
	shdr->findUniVar("texProjectionMat")->set(&texProjectionMat);

	// the shadowmap
	if(light.castsShadow() && sm.isEnabled())
	{
		shdr->findUniVar("shadowMap")->set(sm.getShadowMap(), 5);
		float smSize = sm.getShadowMap().getWidth();
		shdr->findUniVar("shadowMapSize")->set(&smSize);
	}

	// render quad
	r.drawQuad();
}


//======================================================================================================================
// run                                                                                                                 =
//======================================================================================================================
void Is::run()
{
	// FBO
	fbo.bind();

	// OGL stuff
	Renderer::setViewport(0, 0, r.getWidth(), r.getHeight());

	GlStateMachineSingleton::getInstance().setDepthTestEnabled(false);

	// ambient pass
	ambientPass(SceneSingleton::getInstance().getAmbientCol());

	// light passes
	GlStateMachineSingleton::getInstance().setBlendingEnabled(true);
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_STENCIL_TEST);

	// for all lights
	BOOST_FOREACH(const PointLight* light, r.getCamera().getVisiblePointLights())
	{
		pointLightPass(*light);
	}
	
	BOOST_FOREACH(const SpotLight* light, r.getCamera().getVisibleSpotLights())
	{
		spotLightPass(*light);
	}
	

	glDisable(GL_STENCIL_TEST);

	// FBO
	fbo.unbind();

	ON_GL_FAIL_THROW_EXCEPTION();
}
