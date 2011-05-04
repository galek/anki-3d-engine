#ifndef SMO_H
#define SMO_H

#include "RenderingPass.h"
#include "Fbo.h"
#include "ShaderProg.h"
#include "RsrcPtr.h"
#include "Vbo.h"
#include "Vao.h"
#include "Camera.h"


class PointLight;
class SpotLight;


/// Stencil masking optimizations
class Smo: public RenderingPass
{
	public:
		Smo(Renderer& r_): RenderingPass(r_) {}
		void init(const RendererInitializer& initializer);
		void run(const PointLight& light);
		void run(const SpotLight& light);

	private:
		/// @todo
		struct Geom
		{
			RsrcPtr<Mesh> mesh;
			Vao vao;
		};

		Geom sphereGeom;

		/// An array of geometry stuff. For perspective cameras the shape is a pyramid, see the blend file with the
		/// vertex positions
		boost::array<Geom, Camera::CT_NUM> camGeom;

		RsrcPtr<ShaderProg> sProg;

		void initCamGeom();

		void setUpGl(bool inside);
		void restoreGl(bool inside);
};


#endif
