#ifndef PPS_H
#define PPS_H

#include "RenderingPass.h"
#include "Fbo.h"
#include "Texture.h"
#include "RsrcPtr.h"


class Hdr;
class Ssao;
class ShaderProg;


/// Post-processing stage.
///
/// This stage is divided into 2 two parts. The first happens before blending stage and the second after.
class Pps: private RenderingPass
{
	public:
		Texture prePassFai;
		Texture postPassFai;

		Pps(Renderer& r_, Object* parent);
		void init(const RendererInitializer& initializer);
		void runPrePass();
		void runPostPass();

		/// @name Accessors
		/// @{
		Hdr& getHdr() {return *hdr;}
		/// @}

	private:
		/// @name Passes
		/// @{
		Hdr* hdr;
		Ssao* ssao;
		/// @}

		Fbo prePassFbo;
		Fbo postPassFbo;
		RsrcPtr<ShaderProg> prePassSProg;
		RsrcPtr<ShaderProg> postPassSProg;

		void initPassFbo(Fbo& fbo, Texture& fai);

		/// Before BS pass
		void initPrePassSProg();

		/// After BS pass
		void initPostPassSProg();
};


#endif
