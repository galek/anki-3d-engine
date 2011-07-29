#ifndef R_SSAO_H
#define R_SSAO_H

#include "RenderingPass.h"
#include "GfxApi/BufferObjects/Fbo.h"
#include "Resources/Texture.h"
#include "Resources/ShaderProgram.h"
#include "Resources/RsrcPtr.h"
#include "GfxApi/BufferObjects/Vbo.h"
#include "GfxApi/BufferObjects/Vao.h"


namespace R {


/// Screen space ambient occlusion pass
///
/// Three passes:
/// 1) Calc ssao factor
/// 2) Blur vertically
/// 3) Blur horizontally repeat 2, 3
class Ssao: private RenderingPass
{
	public:
		Ssao(Renderer& r_): RenderingPass(r_) {}
		void init(const RendererInitializer& initializer);
		void run();

		/// @name Accessors
		/// @{
		GETTER_R_BY_VAL(bool, enabled, isEnabled)
		GETTER_R_BY_VAL(float, renderingQuality, getRenderingQuality)
		GETTER_R(Texture, fai, getFai)
		/// @}

	private:
		Texture ssaoFai; ///< It contains the unblurred SSAO factor
		Texture hblurFai;
		Texture fai;  ///< AKA vblurFai The final FAI
		bool enabled;
		float renderingQuality;
		float blurringIterationsNum;
		Fbo ssaoFbo;
		Fbo hblurFbo;
		Fbo vblurFbo;
		RsrcPtr<Texture> noiseMap;
		RsrcPtr<ShaderProgram> ssaoSProg;
		RsrcPtr<ShaderProgram> hblurSProg;
		RsrcPtr<ShaderProgram> vblurSProg;

		void createFbo(Fbo& fbo, Texture& fai);
};


} // end namespace


#endif
