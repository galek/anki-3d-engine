#include "anki/scene/Camera.h"

namespace anki {

//==============================================================================
// Camera                                                                      =
//==============================================================================

//==============================================================================
Camera::Camera(
	const char* name, SceneGraph* scene, // SceneNode
	CameraType type_) 
	:	SceneNode(name, scene), 
		MoveComponent(this),
		FrustumComponent(this),
		SpatialComponent(this),
		type(type_)
{
	// Init components
	addComponent(static_cast<MoveComponent*>(this));
	addComponent(static_cast<SpatialComponent*>(this));
	addComponent(static_cast<FrustumComponent*>(this));
}

//==============================================================================
Camera::~Camera()
{}

//==============================================================================
void Camera::lookAtPoint(const Vec3& point)
{
	MoveComponent& move = *this;

	const Vec3& j = Vec3(0.0, 1.0, 0.0);
	Vec3 vdir = (point - move.getLocalTransform().getOrigin()).getNormalized();
	Vec3 vup = j - vdir * j.dot(vdir);
	Vec3 vside = vdir.cross(vup);

	Mat3 rot = move.getLocalTransform().getRotation();
	rot.setColumns(vside, vup, -vdir);
	move.setLocalRotation(rot);
}

//==============================================================================
void Camera::frustumUpdate()
{
	// Frustum
	FrustumComponent& fr = *this;
	fr.setProjectionMatrix(fr.getFrustum().calculateProjectionMatrix());
	fr.setViewProjectionMatrix(fr.getProjectionMatrix() * fr.getViewMatrix());
	fr.markForUpdate();

	// Spatial
	SpatialComponent& sp = *this;
	sp.markForUpdate();
}

//==============================================================================
void Camera::moveUpdate(MoveComponent& move)
{
	// Frustum
	FrustumComponent& fr = *this;
	fr.setViewMatrix(Mat4(move.getWorldTransform().getInverse()));
	fr.setViewProjectionMatrix(fr.getProjectionMatrix() * fr.getViewMatrix());
	fr.markForUpdate();
	fr.getFrustum().setTransform(move.getWorldTransform());

	// Spatial
	SpatialComponent& sp = *this;
	sp.markForUpdate();
}

//==============================================================================
// PerspectiveCamera                                                           =
//==============================================================================

//==============================================================================
PerspectiveCamera::PerspectiveCamera(const char* name, SceneGraph* scene)
	: Camera(name, scene, CT_PERSPECTIVE)
{}

//==============================================================================
// OrthographicCamera                                                          =
//==============================================================================

//==============================================================================
OrthographicCamera::OrthographicCamera(const char* name, SceneGraph* scene)
	: Camera(name, scene, CT_ORTHOGRAPHIC)
{}

} // end namespace anki
