#include "anki/gl/GlException.h"
#include "anki/gl/Gl.h"
#include "anki/util/Exception.h"
#include <cstring>

namespace anki {

void glConditionalThrowException(const char* file, int line, const char* func)
{
	GLenum errId = glGetError();
	if(errId == GL_NO_ERROR)
	{
		return;
	}

	const char* glerr;
	switch(errId)
	{
	case GL_INVALID_ENUM:
		glerr = "GL_INVALID_ENUM";
		break;
	case GL_INVALID_VALUE:
		glerr = "GL_INVALID_VALUE";
		break;
	case GL_INVALID_OPERATION:
		glerr = "GL_INVALID_OPERATION";
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		glerr = "GL_INVALID_FRAMEBUFFER_OPERATION";
		break;
	case GL_OUT_OF_MEMORY:
		glerr = "GL_OUT_OF_MEMORY";
		break;
	default:
		glerr = "unknown";
	};

	char errStr[256];
	const char tmp[] = "OpenGL exception: ";
	memcpy(errStr, tmp, sizeof(tmp));
	strcat(errStr, glerr);
	throw Exception(errStr, file, line, func);
}

} // end namespace
