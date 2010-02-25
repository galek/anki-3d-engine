#ifndef _APP_H_
#define _APP_H_

#include <SDL/SDL.h>
#include "common.h"

namespace app {


extern uint timer_tick;

extern uint desktopW;
extern uint desktopH;

extern uint windowW;
extern uint windowH;

extern void initWindow();
extern void quitApp( int code );
extern void waitForNextFrame();
extern void togleFullScreen();
extern void printAppInfo();

inline uint getTicks()
{
	return SDL_GetTicks();
}


} // end namespace
#endif
