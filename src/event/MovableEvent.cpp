#include "anki/event/MovableEvent.h"
#include "anki/scene/SceneNode.h"
#include "anki/scene/Movable.h"
#include "anki/util/Functions.h"

namespace anki {

//==============================================================================
MovableEvent::MovableEvent(EventManager* manager, F32 startTime, F32 duration,
	U8 flags, SceneNode* movableSceneNode, const MovableEventData& data)
	: Event(manager, startTime, duration, movableSceneNode, flags)
{
	ANKI_ASSERT(data.movableSceneNode->getMovable());

	*static_cast<MovableEventData*>(this) = data;

	originalPos =
		movableSceneNode->getMovable()->getLocalTransform().getOrigin();

	for(U i = 0; i < 3; i++)
	{
		newPos[i] = randRange(posMin[i], posMax[i]);
	}
	newPos += movableSceneNode->getMovable()->getLocalTransform().getOrigin();
}

//==============================================================================
void MovableEvent::update(F32 prevUpdateTime, F32 crntTime)
{
	SceneNode* movableSceneNode = getSceneNode();
	ANKI_ASSERT(movableSceneNode);

	Transform trf = movableSceneNode->getMovable()->getLocalTransform();

	F32 factor = sin(getDelta(crntTime) * getPi<F32>());

	trf.getOrigin() = interpolate(originalPos, newPos, factor);

	movableSceneNode->getMovable()->setLocalTransform(trf);
}

} // end namespace anki
