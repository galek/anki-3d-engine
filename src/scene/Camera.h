#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "common.h"
#include "collision.h"
#include "Node.h"


class Camera: public Node
{
	public:
		enum FrustrumPlanes
		{
			FP_LEFT = 0,
			FP_RIGHT,
			FP_NEAR,
			FP_TOP,
			FP_BOTTOM,
			FP_FAR
		};

		// Fovx is the angle in the y axis (imagine the cam positioned in the default OGL pos)
		// Note that fovX > fovY (most of the time) and aspect_ratio = fovX/fovY
		// fovX and fovY in rad
		float fovX, fovY;
		float zNear, zFar;

		// the frustum planes in local and world space
		plane_t lspaceFrustumPlanes[6];
		plane_t wspaceFrustumPlanes[6];

		// matrices
		Mat4 projectionMat;
		Mat4 viewMat;
		/**
		 * Used in deferred shading for the calculation of view vector (see CalcViewVector). The reason we store this matrix here is
		 * that we dont want it to be re-calculated all the time but only when the projection params (fovX, fovY, zNear, zFar) change.
		 * Fortunately the projection params change rarely. Note that the Camera as we all know re-calculates the matreces only when the
		 * parameteres change!!
		 */
		Mat4 invProjectionMat;

		// misc
		void calcProjectionMatrix();
		void updateViewMatrix();
		void calcLSpaceFrustumPlanes();
		void updateWSpaceFrustumPlanes();

	public:
		// constructors and destuctors
		Camera( float fovx_, float fovy_, float znear_, float zfar_ ): Node(NT_CAMERA), fovX(fovx_), fovY(fovy_), zNear(znear_), zFar(zfar_)
		{
			calcLSpaceFrustumPlanes();
			updateWSpaceFrustumPlanes();
			calcProjectionMatrix();
		}
		Camera( const Camera& c ): Node(NT_CAMERA) { memcpy( this, &c, sizeof( Camera ) ); }
		Camera(): Node(NT_CAMERA) {}
		~Camera() {}

		// Sets & Gets
		void setFovX ( float fovx_ )  { fovX=fovx_; calcProjectionMatrix(); calcLSpaceFrustumPlanes(); }
		void setFovY ( float fovy_ )  { fovY=fovy_; calcProjectionMatrix(); calcLSpaceFrustumPlanes(); }
		void setZNear( float znear_ ) { zNear=znear_; calcProjectionMatrix(); calcLSpaceFrustumPlanes(); }
		void setZFar ( float zfar_ )  { zFar=zfar_; calcProjectionMatrix(); calcLSpaceFrustumPlanes(); }
		void setAll( float fovx_, float fovy_, float znear_, float zfar_ );
		float getFovX () const { return fovX; }
		float getFovY () const { return fovY; }
		float getZNear() const { return zNear; }
		float getZFar () const { return zFar; }
		const Mat4& getProjectionMatrix() const { return projectionMat; }
		const Mat4& getViewMatrix() const { return viewMat; }
		const Mat4& getInvProjectionMatrix() const { return invProjectionMat; } // see the declaration of invProjectionMat for info

		// misc
		void lookAtPoint( const Vec3& point );
		void updateWorldStuff();
		void render();
		void init( const char* ) {}
		void deinit() {}

		// frustum stuff
		bool insideFrustum( const bvolume_t& vol ) const;
		bool insideFrustum( const Camera& cam ) const; // check if another camera is inside our view (used for projected lights)
};


#endif
