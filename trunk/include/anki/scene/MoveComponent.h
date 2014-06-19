// Copyright (C) 2014, Panagiotis Christopoulos Charitos.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

#ifndef ANKI_SCENE_MOVE_COMPONENT_H
#define ANKI_SCENE_MOVE_COMPONENT_H

#include "anki/scene/Common.h"
#include "anki/scene/SceneComponent.h"
#include "anki/util/Bitset.h"
#include "anki/Math.h"

namespace anki {

/// @addtogroup Scene
/// @{

/// Interface for movable scene nodes
class MoveComponent: 
	public SceneComponent,
	public SceneHierarchicalObject<MoveComponent>, 
	public Bitset<U8>
{
public:
	typedef SceneHierarchicalObject<MoveComponent> Base;

	enum MoveComponentFlag
	{
		MF_NONE = 0,

		/// Get the parent's world transform
		MF_IGNORE_LOCAL_TRANSFORM = 1 << 1,

		/// If dirty then is marked for update
		MF_MARKED_FOR_UPDATE = 1 << 3,
	};

	/// @name Constructors & destructor
	/// @{

	/// The one and only constructor
	/// @param node The scene node to steal it's allocators
	/// @param flags The flags
	MoveComponent(SceneNode* node, U32 flags = MF_NONE);

	~MoveComponent();
	/// @}

	/// @name Accessors
	/// @{
	const Transform& getLocalTransform() const
	{
		return m_ltrf;
	}
	void setLocalTransform(const Transform& x)
	{
		m_ltrf = x;
		markForUpdate();
	}
	void setLocalOrigin(const Vec4& x)
	{
		m_ltrf.setOrigin(x);
		markForUpdate();
	}
	const Vec4& getLocalOrigin() const
	{
		return m_ltrf.getOrigin();
	}
	void setLocalRotation(const Mat3x4& x)
	{
		m_ltrf.setRotation(x);
		markForUpdate();
	}
	const Mat3x4& getLocalRotation() const
	{
		return m_ltrf.getRotation();
	}
	void setLocalScale(F32 x)
	{
		m_ltrf.setScale(x);
		markForUpdate();
	}
	F32 getLocalScale() const
	{
		return m_ltrf.getScale();
	}

	const Transform& getWorldTransform() const
	{
		return m_wtrf;
	}

	const Transform& getPrevWorldTransform() const
	{
		return m_prevWTrf;
	}
	/// @}

	/// @name SceneComponent overrides
	/// @{

	/// Update self and children world transform recursively, if root node.
	/// Need to call this at every frame.
	/// @note Don't update if child because we start from roots and go to
	///       children and we don't want a child to be updated before the
	///       parent
	Bool update(SceneNode&, F32, F32, UpdateType uptype) override;
	/// @}

	/// @name Mess with the local transform
	/// @{
	void rotateLocalX(F32 angDegrees)
	{
		m_ltrf.getRotation().rotateXAxis(angDegrees);
		markForUpdate();
	}
	void rotateLocalY(F32 angDegrees)
	{
		m_ltrf.getRotation().rotateYAxis(angDegrees);
		markForUpdate();
	}
	void rotateLocalZ(F32 angDegrees)
	{
		m_ltrf.getRotation().rotateZAxis(angDegrees);
		markForUpdate();
	}
	void moveLocalX(F32 distance)
	{
		Vec3 x_axis = m_ltrf.getRotation().getColumn(0);
		m_ltrf.getOrigin() += Vec4(x_axis, 0.0) * distance;
		markForUpdate();
	}
	void moveLocalY(F32 distance)
	{
		Vec3 y_axis = m_ltrf.getRotation().getColumn(1);
		m_ltrf.getOrigin() += Vec4(y_axis, 0.0) * distance;
		markForUpdate();
	}
	void moveLocalZ(F32 distance)
	{
		Vec3 z_axis = m_ltrf.getRotation().getColumn(2);
		m_ltrf.getOrigin() += Vec4(z_axis, 0.0) * distance;
		markForUpdate();
	}
	void scale(F32 s)
	{
		m_ltrf.getScale() *= s;
		markForUpdate();
	}
	/// @}

	static constexpr Type getClassType()
	{
		return MOVE_COMPONENT;
	}

private:
	SceneNode* m_node;

	/// The transformation in local space
	Transform m_ltrf = Transform::getIdentity();

	/// The transformation in world space (local combined with parent's
	/// transformation)
	Transform m_wtrf = Transform::getIdentity();

	/// Keep the previous transformation for checking if it moved
	Transform m_prevWTrf = Transform::getIdentity();

	void markForUpdate()
	{
		enableBits(MF_MARKED_FOR_UPDATE);
	}

	/// Called every frame. It updates the @a m_wtrf if @a shouldUpdateWTrf
	/// is true. Then it moves to the children.
	void updateWorldTransform();
};
/// @}

} // end namespace anki

#endif
