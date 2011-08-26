#include "Pps.h"
#include "Renderer.h"
#include "Hdr.h"
#include "Ssao.h"
#include "RendererInitializer.h"


namespace r {


//==============================================================================
// Constructor                                                                 =
//==============================================================================
Pps::Pps(Renderer& r_)
:	RenderingPass(r_),
	hdr(r_),
	ssao(r_),
	bl(r_)
{}


//==============================================================================
// Destructor                                                                  =
//==============================================================================
Pps::~Pps()
{}


//==============================================================================
// init                                                                        =
//==============================================================================
void Pps::init(const RendererInitializer& initializer)
{
	ssao.init(initializer);
	hdr.init(initializer);

	//
	// Init pre pass
	//

	// FBO
	try
	{
		prePassFbo.create();
		prePassFbo.bind();
		prePassFbo.setNumOfColorAttachements(1);
		Renderer::createFai(r.getWidth(), r.getHeight(), GL_RGB, GL_RGB,
			GL_FLOAT, prePassFai);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, prePassFai.getGlId(), 0);
		prePassFbo.checkIfGood();
		prePassFbo.unbind();
	}
	catch(std::exception& e)
	{
		throw EXCEPTION("Cannot create pre-pass post-processing stage FBO: " +
			e.what());
	}

	// SProg
	std::string pps = "";
	if(ssao.isEnabled())
	{
		pps += "#define SSAO_ENABLED\n";
	}

	prePassSProg.loadRsrc(ShaderProgram::createSrcCodeToCache(
		"shaders/PpsPrePass.glsl", pps.c_str()).c_str());

	//
	// Init post pass
	//

	// FBO
	try
	{
		postPassFbo.create();
		postPassFbo.bind();
		postPassFbo.setNumOfColorAttachements(1);
		Renderer::createFai(r.getWidth(), r.getHeight(), GL_RGB, GL_RGB,
			GL_FLOAT, postPassFai);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, postPassFai.getGlId(), 0);
		postPassFbo.checkIfGood();
		postPassFbo.unbind();
	}
	catch(std::exception& e)
	{
		throw EXCEPTION("Cannot create post-pass post-processing stage FBO: " +
			e.what());
	}

	// SProg
	pps = "";

	if(hdr.isEnabled())
	{
		pps += "#define HDR_ENABLED\n";
	}

	postPassSProg.loadRsrc(ShaderProgram::createSrcCodeToCache(
		"shaders/PpsPostPass.glsl", pps.c_str()).c_str());

	//
	// Init Bl after
	//
	bl.init(initializer);
}


//==============================================================================
// runPrePass                                                                  =
//==============================================================================
void Pps::runPrePass()
{
	if(ssao.isEnabled())
	{
		ssao.run();
	}

	prePassFbo.bind();

	GlStateMachineSingleton::get().enable(GL_DEPTH_TEST, false);
	GlStateMachineSingleton::get().enable(GL_BLEND, false);
	GlStateMachineSingleton::get().setViewport(0, 0,
		r.getWidth(), r.getHeight());

	prePassSProg->bind();
	prePassSProg->getUniformVariableByName("isFai").set(r.getIs().getFai(), 0);

	if(ssao.isEnabled())
	{
		prePassSProg->getUniformVariableByName("ppsSsaoFai").set(
			ssao.getFai(), 1);
	}

	r.drawQuad();
}


//==============================================================================
// runPostPass                                                                 =
//==============================================================================
void Pps::runPostPass()
{
	//
	// The actual pass
	//
	if(hdr.isEnabled())
	{
		hdr.run();
	}

	postPassFbo.bind();

	GlStateMachineSingleton::get().enable(GL_DEPTH_TEST, false);
	GlStateMachineSingleton::get().enable(GL_BLEND, false);
	GlStateMachineSingleton::get().setViewport(0, 0,
		r.getWidth(), r.getHeight());

	postPassSProg->bind();
	postPassSProg->getUniformVariableByName("ppsPrePassFai").set(
		prePassFai, 0);
	if(hdr.isEnabled())
	{
		postPassSProg->getUniformVariableByName("ppsHdrFai").set(
			hdr.getFai(), 1);
	}

	r.drawQuad();

	//
	// Blurring
	//
	bl.run();
}


} // end namespace
