#ifndef CAMERA_H
#define CAMERA_H

#include <boost/array.hpp>
#include <deque>

#include "Vec.h"
#include "Collision.h"
#include "SceneNode.h"
#include "Properties.h"


class RenderableNode;
class SpotLight;
class PointLight;


/// Camera SceneNode
class Camera: public SceneNode
{
	public:
		enum CameraType
		{
			CT_PERSPECTIVE,
			CT_ORTHOGRAPHIC
		};

		enum FrustrumPlanes
		{
			FP_LEFT = 0,
			FP_RIGHT,
			FP_NEAR,
			FP_TOP,
			FP_BOTTOM,
			FP_FAR,
			FP_NUM
		};

		Camera(bool compoundFlag, SceneNode* parent): SceneNode(SNT_CAMERA, compoundFlag, parent) {}
		~Camera() {}

		/// @name Accessors
		/// @{
		float getFovX() const {return fovX;}
		void setFovX(float fovx);

		float getFovY() const {return fovY;}
		void setFovY(float fovy);

		float getZNear() const {return zNear;}
		void setZNear(float znear);

		float getZFar() const {return zFar;}
		void setZFar(float zfar);

		void setAll(float fovx, float fovy, float znear, float zfar);

		const Mat4& getProjectionMatrix() const {return projectionMat;}
		const Mat4& getViewMatrix() const {return viewMat;}

		/// See the declaration of invProjectionMat for info
		const Mat4& getInvProjectionMatrix() const {return invProjectionMat;}

		GETTER_RW(std::deque<const RenderableNode*>, msRenderableNodes, getVisibleMsRenderableNodes)
		GETTER_RW(std::deque<const RenderableNode*>, bsRenderableNodes, getVisibleBsRenderableNodes)
		GETTER_RW(Vec<const PointLight*>, pointLights, getVisiblePointLights)
		GETTER_RW(Vec<SpotLight*>, spotLights, getVisibleSpotLights)
		/// @}

		void lookAtPoint(const Vec3& point);

		/// This does:
		/// - Update view matrix
		/// - Update frustum planes
		void moveUpdate();

		/// Do nothing
		void frameUpdate() {}

		/// Do nothing
		void init(const char*) {}

		/// @name Frustum checks
		/// @{

		/// Check if the given camera is inside the frustum clipping planes. This is used mainly to test if the projected
		/// lights are visible
		bool insideFrustum(const CollisionShape& vol) const;

		/// Check if another camera is inside our view (used for projected lights)
		bool insideFrustum(const Camera& cam) const;
		/// @}

	public:
		/// @name Angles
		/// fovX is the angle in the y axis (imagine the cam positioned in the default OGL pos) Note that fovX > fovY
		/// (most of the time) and aspectRatio = fovX/fovY
		/// @{
		float fovX, fovY;
		/// @}

		float zNear, zFar;

		/// @name The frustum planes in local and world space
		/// @{
		boost::array<Plane, FP_NUM> lspaceFrustumPlanes;
		boost::array<Plane, FP_NUM> wspaceFrustumPlanes;
		/// @}

		/// @name Matrices
		/// @{
		Mat4 projectionMat;
		Mat4 viewMat;

		/// Used in deferred shading for the calculation of view vector (see CalcViewVector). The reason we store this
		/// matrix here is that we dont want it to be re-calculated all the time but only when the projection params
		/// (fovX, fovY, zNear, zFar) change. Fortunately the projection params change rarely. Note that the Camera as
		/// we all know re-calculates the matrices only when the parameters change!!
		Mat4 invProjectionMat;
		/// @}

		/// @name Visible nodes. They are in separate containers for faster shorting
		/// @{
		std::deque<const RenderableNode*> msRenderableNodes;
		std::deque<const RenderableNode*> bsRenderableNodes;
		Vec<const PointLight*> pointLights;
		Vec<SpotLight*> spotLights;
		/// @}

		void calcProjectionMatrix();
		void updateViewMatrix();
		void calcLSpaceFrustumPlanes();
		void updateWSpaceFrustumPlanes();
};


inline void Camera::setFovX(float fovx_)
{
	fovX = fovx_;
	calcProjectionMatrix();
	calcLSpaceFrustumPlanes();
}


inline void Camera::setFovY(float fovy_)
{
	fovY = fovy_;
	calcProjectionMatrix();
	calcLSpaceFrustumPlanes();
}


inline void Camera::setZNear(float znear_)
{
	zNear = znear_;
	calcProjectionMatrix();
	calcLSpaceFrustumPlanes();
}


inline void Camera::setZFar(float zfar_)
{
	zFar = zfar_;
	calcProjectionMatrix();
	calcLSpaceFrustumPlanes();
}


#endif
