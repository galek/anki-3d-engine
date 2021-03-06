// Copyright (C) 2014, Panagiotis Christopoulos Charitos.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

/// @file
/// This file is included by all the .cpp files that wrap something

#ifndef ANKI_SCRIPT_COMMON_H
#define ANKI_SCRIPT_COMMON_H

#include "anki/script/LuaBinder.h"

/// Wrap a class
#define ANKI_SCRIPT_WRAP(x) \
	void ankiScriptWrap##x(LuaBinder& lb)

/// XXX
#define ANKI_SCRIPT_CALL_WRAP(x) \
	extern void ankiScriptWrap##x(LuaBinder&); \
	ankiScriptWrap##x(*this);

/// XXX
#define ANKI_SCRIPT_WRAP_SINGLETON(x) \
	ANKI_SCRIPT_WRAP(x) { \
	ANKI_LUA_CLASS_BEGIN_NO_DESTRUCTOR(lb, x)	\
		ANKI_LUA_STATIC_METHOD("get", &x::get) \
	ANKI_LUA_CLASS_END() }

#endif
