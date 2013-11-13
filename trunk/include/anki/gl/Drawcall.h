#ifndef ANKI_GL_DRAWCALL_H
#define ANKI_GL_DRAWCALL_H

#include "anki/gl/Ogl.h"
#include "anki/util/StdTypes.h"

namespace anki {

/// @addtogroup OpenGL
/// @{

/// A GL drawcall
struct Drawcall
{
	GLenum primitiveType = 0;
	GLenum indicesType = 0;
	U32 instancesCount = 1;
	U32 drawcallCount = 1;
	Array<U32, ANKI_MAX_MULTIDRAW_PRIMITIVES> indicesCountArray;
	Array<PtrSize, ANKI_MAX_MULTIDRAW_PRIMITIVES> offsetsArray;

	/// Execute the dracall
	void enque();
};
/// @}

} // end namespace anki

#endif
