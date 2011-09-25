#ifndef PROJECTION_CAMERA_FRUSTUM_H
#define PROJECTION_CAMERA_FRUSTUM_H

#include "CollisionShape.h"
#include "m/Math.h"


/// @addtogroup Collision
/// @{

/// Collision shape for the projection cameras
class ProjectionCameraShape: public CollisionShape
{
	public:
		/// Default constructor
		ProjectionCameraShape()
		:	CollisionShape(CST_PROJECTION_CAMERA_FRUSTRUM)
		{}

		ProjectionCameraShape(float fovX, float fovY, float zNear,
			float zFar, const Transform& trf)
		:	CollisionShape(CST_PROJECTION_CAMERA_FRUSTRUM)
		{
			setAll(fovX, fovY, zNear, zFar, trf);
		}

		/// @copydoc CollisionShape::testPlane
		float testPlane(const Plane& p) const;

		ProjectionCameraShape getTransformed(const Transform& trf) const;

		/// Set all
		void setAll(float fovX, float fovY, float zNear,
			float zFar, const Transform& trf);


	private:
		Vec3 eye; ///< The eye point
		boost::array<Vec3, 4> dirs; ///< Directions
};
///@}


#endif
