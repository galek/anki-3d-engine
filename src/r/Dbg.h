#ifndef R_DBG_H
#define R_DBG_H

#include <boost/array.hpp>
#include <map>
#include "RenderingPass.h"
#include "gl/Fbo.h"
#include "rsrc/ShaderProgram.h"
#include "rsrc/RsrcPtr.h"
#include "m/Math.h"
#include "gl/Vbo.h"
#include "gl/Vao.h"
#include "SceneDbgDrawer.h"
#include "CollisionDbgDrawer.h"
#include "util/Accessors.h"


namespace r {


/// Debugging stage
class Dbg: public RenderingPass
{
	public:
		Dbg(Renderer& r_);
		void init(const RendererInitializer& initializer);
		void run();

		void renderGrid();
		void drawSphere(float radius, int complexity = 4);
		void drawCube(float size = 1.0);
		void drawLine(const Vec3& from, const Vec3& to, const Vec4& color);

		/// @name Accessors
		/// @{
		GETTER_SETTER_BY_VAL(bool, enabled, isEnabled, setEnabled)
		GETTER_SETTER_BY_VAL(bool, showSkeletonsEnabled,
			isShowSkeletonsEnabled, setShowSkeletonsEnabled)
		/// @todo add others
		/// @}

		/// @name Render functions. Imitate the GL 1.1 immediate mode
		/// @{
		void begin(); ///< Initiates the draw
		void end(); ///< Draws
		void pushBackVertex(const Vec3& pos); ///< Something like glVertex
		/// Something like glColor
		void setColor(const Vec3& col) {crntCol = col;}
		/// Something like glColor
		void setColor(const Vec4& col) {crntCol = Vec3(col);}
		void setModelMat(const Mat4& modelMat);
		/// @}

	private:
		bool enabled;
		bool showAxisEnabled;
		bool showLightsEnabled;
		bool showSkeletonsEnabled;
		bool showCamerasEnabled;
		bool showVisibilityBoundingShapesFlag;
		Fbo fbo;
		RsrcPtr<ShaderProgram> sProg;
		static const uint MAX_POINTS_PER_DRAW = 256;
		boost::array<Vec3, MAX_POINTS_PER_DRAW> positions;
		boost::array<Vec3, MAX_POINTS_PER_DRAW> colors;
		Mat4 modelMat;
		uint pointIndex;
		Vec3 crntCol;
		Vbo positionsVbo;
		Vbo colorsVbo;
		Vao vao;
		SceneDbgDrawer sceneDbgDrawer;
		CollisionDbgDrawer collisionDbgDrawer;


		/// This is a container of some precalculated spheres. Its a map that
		/// from sphere complexity it returns a vector of lines (Vec3s in
		/// pairs)
		std::map<uint, Vec<Vec3> > complexityToPreCalculatedSphere;
};


} // end namespace


#endif