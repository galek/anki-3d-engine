// Copyright (C) 2014, Panagiotis Christopoulos Charitos.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

#ifndef ANKI_SCENE_INSTANCE_NODE_H
#define ANKI_SCENE_INSTANCE_NODE_H

#include "anki/scene/SceneNode.h"

namespace anki {

/// @addtogroup Scene
/// @{

/// Instance component. Dummy used only for idendification
class InstanceComponent: public SceneComponent
{
public:
	InstanceComponent(SceneNode* node)
		: SceneComponent(INSTANCE_COMPONENT, node)
	{}

	static constexpr Type getClassType()
	{
		return INSTANCE_COMPONENT;
	}
};

/// Instance scene node
class InstanceNode: public SceneNode, public InstanceComponent, 
	public MoveComponent
{
public:
	InstanceNode(const char* name, SceneGraph* scene)
		: SceneNode(name, scene), InstanceComponent(this), MoveComponent(this)
	{
		addComponent(static_cast<InstanceComponent*>(this));
		addComponent(static_cast<MoveComponent*>(this));
	}
};

/// @}

} // end namespace anki

#endif

