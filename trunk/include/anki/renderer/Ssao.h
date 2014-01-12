#ifndef ANKI_RENDERER_SSAO_H
#define ANKI_RENDERER_SSAO_H

#include "anki/renderer/RenderingPass.h"
#include "anki/resource/ShaderProgramResource.h"
#include "anki/resource/TextureResource.h"
#include "anki/resource/Resource.h"
#include "anki/gl/Fbo.h"
#include "anki/gl/Texture.h"
#include "anki/gl/GlBuffer.h"
#include "anki/core/Timestamp.h"

namespace anki {

/// Screen space ambient occlusion pass
///
/// Three passes:
/// @li Calc ssao factor
/// @li Blur vertically
/// @li Blur horizontally
/// @li Repeat from 2
class Ssao: public SwitchableRenderingPass
{
public:
	Ssao(Renderer* r_)
		: SwitchableRenderingPass(r_)
	{}

	void init(const RendererInitializer& initializer);
	void run();

	/// @name Accessors
	/// @{
	const Texture& getFai() const
	{
		return vblurFai;
	}
	/// @}

private:
	U32 width, height; ///< Blur passes size
	Texture vblurFai;
	Texture hblurFai;
	U32 blurringIterationsCount;
	Fbo vblurFbo;
	Fbo hblurFbo;
	Texture noiseTex;
	ShaderProgramResourcePointer ssaoSProg;
	ShaderProgramResourcePointer hblurSProg;
	ShaderProgramResourcePointer vblurSProg;
	Timestamp commonUboUpdateTimestamp = getGlobTimestamp();
	GlBuffer commonUbo;

	static void createFbo(Fbo& fbo, Texture& fai, U width, U height);
	void initInternal(const RendererInitializer& initializer);
};

} // end namespace anki

#endif
