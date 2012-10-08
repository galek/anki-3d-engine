#include "anki/core/App.h"
#include "anki/core/Logger.h"
#include "anki/util/Exception.h"
#include "anki/util/Platform.h"
#include "anki/util/Filesystem.h"
#include "anki/Config.h"
#include "anki/util/Platform.h"
#include <GL/glew.h>
#include <cstring>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace anki {

//==============================================================================
void App::handleLoggerMessages(const Logger::Info& info)
{
	std::ostream* out = NULL;
	const char* x = NULL;

	switch(info.type)
	{
	case Logger::LMT_NORMAL:
		out = &std::cout;
		x = "Info";
		break;

	case Logger::LMT_ERROR:
		out = &std::cerr;
		x = "Error";
		break;

	case Logger::LMT_WARNING:
		out = &std::cerr;
		x = "Warn";
		break;
	}

	(*out) << "(" << info.file << ":" << info.line << " "<< info.func 
		<< ") " << x << ": " << info.msg << std::endl;
}

//==============================================================================
void App::parseCommandLineArgs(int argc, char* argv[])
{
#if 0
	for(int i = 1; i < argc; i++)
	{
		char* arg = argv[i];
		if(strcmp(arg, "--terminal-coloring") == 0)
		{
			terminalColoringEnabled = true;
		}
		else if(strcmp(arg, "--no-terminal-coloring") == 0)
		{
			terminalColoringEnabled = false;
		}
		else
		{
			std::cerr << "Incorrect command line argument: " << arg
				<< std::endl;
			abort();
		}
	}
#endif
}

//==============================================================================
void App::init(int argc, char* argv[])
{
	// send output to handleMessageHanlderMsgs
	ANKI_CONNECT(&LoggerSingleton::get(), messageRecieved, 
		this, handleLoggerMessages);

	parseCommandLineArgs(argc, argv);
	printAppInfo();
	initDirs();

	timerTick = 1.0 / 60.0; // in sec. 1.0 / period
}

//==============================================================================
void App::initDirs()
{
	settingsPath = std::string(getenv("HOME")) + "/.anki";
	if(!directoryExists(settingsPath.c_str()))
	{
		ANKI_LOGI("Creating settings dir: " << settingsPath);
		createDirectory(settingsPath.c_str());
	}

	cachePath = settingsPath + "/cache";
	if(directoryExists(cachePath.c_str()))
	{
		ANKI_LOGI("Deleting dir: " << cachePath);
		removeDirectory(cachePath.c_str());
	}

	ANKI_LOGI("Creating cache dir: " << cachePath);
	createDirectory(cachePath.c_str());
}

//==============================================================================
void App::quit(int code)
{
#if 0
	SDL_FreeSurface(iconImage);
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(windowId);
	SDL_Quit();
	exit(code);
#endif
}

//==============================================================================
void App::printAppInfo()
{
	std::stringstream msg;
	msg << "App info: ";
	msg << "Version " << ANKI_VERSION_MAJOR << "." << ANKI_VERSION_MINOR 
		<< ", ";
#if NDEBUG
	msg << "Release";
#else
	msg << "Debug";
#endif
	msg << " build, ";

	msg << "platform " << 
#if ANKI_PLATFORM_LINUX
	"Linux"
#elif ANKI_PLATFORM_WINDOWS
	"Windows"
#elif ANKI_PLATFORM_APPLE
	"Apple"
#else
#	error "See file"
#endif
	<< ", ";

	msg << "GLEW " << glewGetString(GLEW_VERSION) << ", ";
	msg << "build date " __DATE__ ", " << "rev " << ANKI_REVISION;

	ANKI_LOGI(msg.str());
}

} // end namespace anki
