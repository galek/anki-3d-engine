#include "anki/event/Event.h"
#include "anki/resource/Resource.h"

namespace anki {

/// @addtogroup Event
/// @{

/// Event controled by animation resource
class AnimationEvent: public Event
{
public:
	AnimationEvent(const AnimationResourcePointer& anim, 
		SceneNode* movableSceneNode);

	/// Implements Event::update
	void update(F32 prevUpdateTime, F32 crntTime);

private:
	AnimationResourcePointer anim;
};
/// @}

} // end namespace anki
