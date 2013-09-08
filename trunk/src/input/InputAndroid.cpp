#include "anki/input/Input.h"
#include "anki/core/NativeWindowAndroid.h"
#include "anki/core/Logger.h"
#include "anki/core/App.h"

namespace anki {

//==============================================================================
// Other                                                                       =
//==============================================================================

//==============================================================================
static void handleAndroidEvents(android_app* app, int32_t cmd)
{
	Input* input = (Input*)app->userData;
	ANKI_ASSERT(input != nullptr);

	switch(cmd)
	{
	case APP_CMD_TERM_WINDOW:
	case APP_CMD_LOST_FOCUS:
		ANKI_LOGI("New event 0x%x", cmd);
		input->addEvent(Input::WINDOW_CLOSED_EVENT);
		break;
	}
}

//==============================================================================
// Input                                                                       =
//==============================================================================

//==============================================================================
Input::~Input()
{}

//==============================================================================
void Input::handleEvents()
{
	int ident;
	int outEvents;
	android_poll_source* source;

	memset(&events[0], 0, sizeof(events));

	while((ident = ALooper_pollAll(0, NULL, &outEvents, (void**)&source)) >= 0) 
	{
		if(source != NULL) 
		{
			source->process(gAndroidApp, source);
		}
	}
}

//==============================================================================
void Input::init(NativeWindow* /*nativeWindow*/)
{
	ANKI_ASSERT(gAndroidApp);
	gAndroidApp->userData = this;
    gAndroidApp->onAppCmd = handleAndroidEvents;
}

//==============================================================================
void Input::moveCursor(const Vec2& posNdc)
{
	// do nothing
}

//==============================================================================
void Input::hideCursor(Bool hide)
{
	// do nothing
}

} // end namespace anki
