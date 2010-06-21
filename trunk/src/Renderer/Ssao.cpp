/**
 * @file
 *
 * Post-processing stage screen space ambient occlusion pass
 */

#include "Renderer.h"
#include "Camera.h"


//======================================================================================================================
// initBlurFbos                                                                                                        =
//======================================================================================================================
void Renderer::Pps::Ssao::initBlurFbo(Fbo& fbo, Texture& fai)
{
	// create FBO
	fbo.create();
	fbo.bind();

	// inform in what buffers we draw
	fbo.setNumOfColorAttachements(1);

	// create the texes
	fai.createEmpty2D(bwidth, bheight, GL_ALPHA8, GL_ALPHA, GL_FLOAT, false);
	fai.setTexParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	fai.setTexParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// attach
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fai.getGlId(), 0);

	// test if success
	if(!fbo.isGood())
		FATAL("Cannot create deferred shading post-processing stage SSAO blur FBO");

	// unbind
	fbo.unbind();
}


//======================================================================================================================
// init                                                                                                                =
//======================================================================================================================
void Renderer::Pps::Ssao::init()
{
	width = renderingQuality * r.width;
	height = renderingQuality * r.height;
	bwidth = height * bluringQuality;
	bheight = height * bluringQuality;


	//
	// init FBOs
	//

	// create FBO
	pass0Fbo.create();
	pass0Fbo.bind();

	// inform in what buffers we draw
	pass0Fbo.setNumOfColorAttachements(1);

	// create the FAI
	pass0Fai.createEmpty2D(width, height, GL_ALPHA8, GL_ALPHA, GL_FLOAT, false);
	pass0Fai.setTexParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	pass0Fai.setTexParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// attach
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pass0Fai.getGlId(), 0);

	// test if success
	if(!pass0Fbo.isGood())
		FATAL("Cannot create deferred shading post-processing stage SSAO pass FBO");

	// unbind
	pass0Fbo.unbind();

	initBlurFbo(pass1Fbo, pass1Fai);
	initBlurFbo(pass2Fbo, fai);


	//
	// Shaders
	//

	ssaoSProg.customLoad("shaders/PpsSsao.glsl");
	blurSProg.customLoad("shaders/PpsSsaoBlur.glsl", ("#define _PPS_SSAO_PASS_0_\n#define PASS0_FAI_WIDTH " +
	                      Util::floatToStr(width) + "\n").c_str());
	blurSProg2.customLoad("shaders/PpsSsaoBlur.glsl", ("#define _PPS_SSAO_PASS_1_\n#define PASS1_FAI_HEIGHT " +
	                       Util::floatToStr(bheight) + "\n").c_str());

	ssaoSProg.uniVars.camerarange = ssaoSProg.findUniVar("camerarange");
	ssaoSProg.uniVars.msDepthFai = ssaoSProg.findUniVar("msDepthFai");
	ssaoSProg.uniVars.noiseMap = ssaoSProg.findUniVar("noiseMap");
	ssaoSProg.uniVars.msNormalFai = ssaoSProg.findUniVar("msNormalFai");
	blurSProg.uniVars.fai = blurSProg.findUniVar("tex"); /// @todo rename the tex in the shader
	blurSProg2.uniVars.fai = blurSProg2.findUniVar("tex"); /// @todo rename the tex in the shader


	//
	// noise map
	//

	/// @todo fix this crap
	// load noise map and disable temporally the texture compression and enable mipmapping
	bool texCompr = Texture::compressionEnabled;
	bool mipmaping = Texture::mipmappingEnabled;
	Texture::compressionEnabled = false;
	Texture::mipmappingEnabled = true;
	noiseMap = Resource::textures.load("gfx/noise3.tga");
	noiseMap->setTexParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	noiseMap->setTexParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	//noise_map->setTexParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//noise_map->setTexParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	Texture::compressionEnabled = texCompr;
	Texture::mipmappingEnabled = mipmaping;

}


//======================================================================================================================
// run                                                                                                                 =
//======================================================================================================================
void Renderer::Pps::Ssao::run()
{
	const Camera& cam = *r.cam;

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);


	// 1st pass
	Renderer::setViewport(0, 0, width, height);
	pass0Fbo.bind();
	ssaoSProg.bind();
	Vec2 camRange(cam.getZNear(), cam.getZFar());
	ssaoSProg.uniVars.camerarange->setVec2(&camRange);
	ssaoSProg.uniVars.msDepthFai->setTexture(r.ms.depthFai, 0);
	ssaoSProg.uniVars.noiseMap->setTexture(*noiseMap, 1);
	ssaoSProg.uniVars.msNormalFai->setTexture(r.ms.normalFai, 2);
	Renderer::drawQuad(0);

	// for 2nd and 3rd passes
	Renderer::setViewport(0, 0, bwidth, bheight);

	// 2nd pass
	pass1Fbo.bind();
	blurSProg.bind();
	blurSProg.uniVars.fai->setTexture(pass0Fai, 0);
	Renderer::drawQuad(0);

	// 3rd pass
	pass2Fbo.bind();
	blurSProg2.bind();
	blurSProg2.uniVars.fai->setTexture(pass1Fai, 0);
	Renderer::drawQuad(0);

	// end
	Fbo::unbind();
}
