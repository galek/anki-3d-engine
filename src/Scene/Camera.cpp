#include <boost/foreach.hpp>
#include "Camera.h"


//======================================================================================================================
// setAll                                                                                                              =
//======================================================================================================================
void Camera::setAll(float fovx_, float fovy_, float znear_, float zfar_)
{
	fovX = fovx_;
	fovY = fovy_;
	zNear = znear_;
	zFar = zfar_;
	calcProjectionMatrix();
	calcLSpaceFrustumPlanes();
}


//======================================================================================================================
// lookAtPoint                                                                                                         =
//======================================================================================================================
void Camera::lookAtPoint(const Vec3& point)
{
	const Vec3& j = Vec3(0.0, 1.0, 0.0);
	Vec3 vdir = (point - getLocalTransform().getOrigin()).getNormalized();
	Vec3 vup = j - vdir * j.dot(vdir);
	Vec3 vside = vdir.cross(vup);
	getLocalTransform().getRotation().setColumns(vside, vup, -vdir);
}


//======================================================================================================================
// calcLSpaceFrustumPlanes                                                                                             =
//======================================================================================================================
void Camera::calcLSpaceFrustumPlanes()
{
	float c, s; // cos & sine

	sinCos(PI + fovX / 2, s, c);
	// right
	lspaceFrustumPlanes[FP_RIGHT] = Plane(Vec3(c, 0.0, s), 0.0);
	// left
	lspaceFrustumPlanes[FP_LEFT] = Plane(Vec3(-c, 0.0, s), 0.0);

	sinCos((3 * PI - fovY) * 0.5, s, c);
	// top
	lspaceFrustumPlanes[FP_TOP] = Plane(Vec3(0.0, s, c), 0.0);
	// bottom
	lspaceFrustumPlanes[FP_BOTTOM] = Plane(Vec3(0.0, -s, c), 0.0);

	// near
	lspaceFrustumPlanes[FP_NEAR] = Plane(Vec3(0.0, 0.0, -1.0), zNear);
	// far
	lspaceFrustumPlanes[FP_FAR] = Plane(Vec3(0.0, 0.0, 1.0), -zFar);
}


//======================================================================================================================
// updateWSpaceFrustumPlanes                                                                                           =
//======================================================================================================================
void Camera::updateWSpaceFrustumPlanes()
{
	for(uint i = 0; i < FP_NUM; i++)
	{
		wspaceFrustumPlanes[i] = lspaceFrustumPlanes[i].getTransformed(getWorldTransform().getOrigin(),
		                                                               getWorldTransform().getRotation(),
		                                                               getWorldTransform().getScale());
	}
}


//======================================================================================================================
// insideFrustum                                                                                                       =
//======================================================================================================================
bool Camera::insideFrustum(const CollisionShape& bvol) const
{
	BOOST_FOREACH(const Plane& plane, wspaceFrustumPlanes)
	{
		if(bvol.testPlane(plane) < 0.0)
		{
			return false;
		}
	}

	return true;
}


//======================================================================================================================
// insideFrustum                                                                                                       =
//======================================================================================================================
bool Camera::insideFrustum(const Camera& cam) const
{
	// get five points. These points are the tips of the given camera
	boost::array<Vec3, 5> points;

	// get 3 sample floats
	float x = cam.getZFar() / tan((PI - cam.getFovX()) / 2.0);
	float y = tan(cam.getFovY() / 2.0) * cam.getZFar();
	float z = -cam.getZFar();

	// the actual points in local space
	points[0] = Vec3(x, y, z); // top right
	points[1] = Vec3(-x, y, z); // top left
	points[2] = Vec3(-x, -y, z); // bottom left
	points[3] = Vec3(x, -y, z); // bottom right
	points[4] = cam.getWorldTransform().getOrigin(); // eye (already in world space)

	// transform them to the given camera's world space (exept the eye)
	for(uint i = 0; i < 4; i++)
	{
		points[i].transform(cam.getWorldTransform());
	}

	// the collision code
	for(uint i = 0; i < 6; i++) // for the 6 planes
	{
		int failed = 0;

		for(uint j = 0; j < 5; j++) // for the 5 points
		{
			if(wspaceFrustumPlanes[i].test(points[j]) < 0.0)
			{
				++failed;
			}
		}
		if(failed == 5)
		{
			return false; // if all points are behind the plane then the cam is not in frustum
		}
	}

	return true;
}


//======================================================================================================================
// calcProjectionMatrix                                                                                                =
//======================================================================================================================
void Camera::calcProjectionMatrix()
{
	float f = 1.0 / tan(fovY * 0.5); // f = cot(fovY/2)

	projectionMat(0, 0) = f * fovY / fovX; // = f/aspectRatio;
	projectionMat(0, 1) = 0.0;
	projectionMat(0, 2) = 0.0;
	projectionMat(0, 3) = 0.0;
	projectionMat(1, 0) = 0.0;
	projectionMat(1, 1) = f;
	projectionMat(1, 2) = 0.0;
	projectionMat(1, 3) = 0.0;
	projectionMat(2, 0) = 0.0;
	projectionMat(2, 1) = 0.0;
	projectionMat(2, 2) = (zFar + zNear) / ( zNear - zFar);
	projectionMat(2, 3) = (2.0 * zFar * zNear) / (zNear - zFar);
	projectionMat(3, 0) = 0.0;
	projectionMat(3, 1) = 0.0;
	projectionMat(3, 2) = -1.0;
	projectionMat(3, 3) = 0.0;

	invProjectionMat = projectionMat.getInverse();
}


//======================================================================================================================
// updateViewMatrix                                                                                                    =
//======================================================================================================================
void Camera::updateViewMatrix()
{
	/*
	 * The point at which the camera looks:
	 * Vec3 viewpoint = translationLspace + z_axis;
	 * as we know the up vector, we can easily use gluLookAt:
	 * gluLookAt(translationLspace.x, translationLspace.x, translationLspace.z, z_axis.x, z_axis.y, z_axis.z, y_axis.x,
	 *           y_axis.y, y_axis.z);
	*/

	// The view matrix is: Mview = camera.world_transform.Inverted(). Bus instead of inverting we do the following:
	Mat3 camInvertedRot = getWorldTransform().getRotation().getTransposed();
	Vec3 camInvertedTsl = -(camInvertedRot * getWorldTransform().getOrigin());
	viewMat = Mat4(camInvertedTsl, camInvertedRot);
}


//======================================================================================================================
// moveUpdate                                                                                                          =
//======================================================================================================================
void Camera::moveUpdate()
{
	updateViewMatrix();
	updateWSpaceFrustumPlanes();
}

