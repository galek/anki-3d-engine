#include "anki/renderer/Pps.h"
#include "anki/renderer/Renderer.h"
#include "anki/renderer/Hdr.h"
#include "anki/renderer/Ssao.h"

namespace anki {

//==============================================================================
Pps::Pps(Renderer* r_)
	: RenderingPass(r_), hdr(r_), ssao(r_), bl(r_)
{}

//==============================================================================
Pps::~Pps()
{}

//==============================================================================
void Pps::init(const Renderer::Initializer& initializer)
{
	ssao.init(initializer);
	hdr.init(initializer);

	// Init pre pass
	//

	// FBO
	try
	{
		Renderer::createFai(r->getWidth(), r->getHeight(), GL_RGB, GL_RGB,
			GL_FLOAT, prePassFai);

		prePassFbo.create();
		prePassFbo.setColorAttachments({&prePassFai});
		ANKI_ASSERT(prePassFbo.isComplete());
	}
	catch(std::exception& e)
	{
		throw ANKI_EXCEPTION("Cannot create pre-pass "
			"post-processing stage FBO") << e;
	}

	// SProg
	std::string pps = "";
	if(ssao.getEnabled())
	{
		pps += "#define SSAO_ENABLED\n";
	}

	prePassSProg.load("shaders/PpsPrePass.glsl", pps.c_str());

	// Init post pass
	//

	// FBO
	try
	{
		Renderer::createFai(r->getWidth(), r->getHeight(), GL_RGB, GL_RGB,
			GL_FLOAT, postPassFai);

		postPassFbo.create();
		postPassFbo.setColorAttachments({&postPassFai});
		ANKI_ASSERT(postPassFbo.isComplete());
	}
	catch(std::exception& e)
	{
		throw ANKI_EXCEPTION("Cannot create post-pass "
			"post-processing stage FBO") << e;
	}

	// SProg
	pps = "";
	if(hdr.getEnabled())
	{
		pps += "#define HDR_ENABLED\n";
	}

	postPassSProg.load("shaders/PpsPostPass.glsl", pps.c_str());

	// Init Bl after
	//
	bl.init(initializer);
}

//==============================================================================
void Pps::runPrePass()
{
	if(ssao.getEnabled())
	{
		ssao.run();
	}

	prePassFbo.bind();

	GlStateSingleton::get().enable(GL_DEPTH_TEST, false);
	GlStateSingleton::get().enable(GL_BLEND, false);
	GlStateSingleton::get().setViewport(0, 0,
		r->getWidth(), r->getHeight());

	prePassSProg.bind();
	prePassSProg.findUniformVariable("isFai").set(r->getIs().getFai());

	if(ssao.getEnabled())
	{
		prePassSProg.findUniformVariable("ppsSsaoFai").set(
			ssao.getFai());
	}

	r->drawQuad();
}

//==============================================================================
void Pps::runPostPass()
{
	// The actual pass
	//
	if(hdr.getEnabled())
	{
		hdr.run();
	}

	postPassFbo.bind();

	GlStateSingleton::get().enable(GL_DEPTH_TEST, false);
	GlStateSingleton::get().enable(GL_BLEND, false);
	GlStateSingleton::get().setViewport(0, 0, r->getWidth(), r->getHeight());

	postPassSProg.bind();
	postPassSProg.findUniformVariable("ppsPrePassFai").set(prePassFai);

	if(hdr.getEnabled())
	{
		postPassSProg.findUniformVariable("ppsHdrFai").set(hdr.getFai());
	}

	r->drawQuad();

	// Blurring
	//
	bl.run();
}

} // end namespace
