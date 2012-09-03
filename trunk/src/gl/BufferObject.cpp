#include <cstring>
#include "anki/gl/BufferObject.h"
#include "anki/gl/GlException.h"
#include "anki/util/Exception.h"

namespace anki {

//==============================================================================

const thread_local BufferObject* BufferObject::lastBindedBo = nullptr;

//==============================================================================
BufferObject::~BufferObject()
{
	if(isCreated())
	{
		destroy();
	}
}

//==============================================================================
void BufferObject::create(GLenum target_, U32 sizeInBytes_,
	const void* dataPtr, GLenum usage_)
{
	ANKI_ASSERT(!isCreated());

	ANKI_ASSERT(usage_ == GL_STREAM_DRAW
		|| usage_ == GL_STATIC_DRAW
		|| usage_ == GL_DYNAMIC_DRAW);

	ANKI_ASSERT(sizeInBytes_ > 0 && "Unacceptable sizeInBytes");

	usage = usage_;
	target = target_;
	sizeInBytes = sizeInBytes_;

	glGenBuffers(1, &glId);
	// XXX Check if zero
	bind();
	glBufferData(target, sizeInBytes, dataPtr, usage);

	// make a check
	int bufferSize = 0;
	glGetBufferParameteriv(target, GL_BUFFER_SIZE, &bufferSize);
	if(sizeInBytes != (uint)bufferSize)
	{
		destroy();
		throw ANKI_EXCEPTION("Data size mismatch");
	}

	unbind();
	ANKI_CHECK_GL_ERROR();
}

//==============================================================================
void* BufferObject::map(U32 offset, U32 length, GLuint flags)
{
	ANKI_ASSERT(mapped == false);
	bind();
	ANKI_ASSERT(offset + length <= sizeInBytes);
	void* mappedMem = glMapBufferRange(target, offset, length, flags);
	ANKI_ASSERT(mappedMem != nullptr);
#if !NDEBUG
	mapped = true;
#endif
	return mappedMem;
}

//==============================================================================
void BufferObject::unmap()
{
	ANKI_ASSERT(mapped == true);
	bind();
	glUnmapBuffer(target);
#if !NDEBUG
	mapped = false;
#endif
}

//==============================================================================
void BufferObject::write(void* buff)
{
	ANKI_ASSERT(isCreated());
	ANKI_ASSERT(usage != GL_STATIC_DRAW);
	bind();
#if 0
	void* mapped = glMapBuffer(target, GL_WRITE_ONLY);
	memcpy(mapped, buff, sizeInBytes);
	glUnmapBuffer(target);
#else
	glBufferData(target, sizeInBytes, buff, usage);
#endif
}

//==============================================================================
void BufferObject::write(void* buff, U32 offset, U32 size)
{
	ANKI_ASSERT(isCreated());
	ANKI_ASSERT(usage != GL_STATIC_DRAW);
	ANKI_ASSERT(offset + size <= sizeInBytes);
	bind();
	void* mapped = glMapBufferRange(target, offset, size,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT/*
		| GL_MAP_FLUSH_EXPLICIT_BIT*/);
	ANKI_ASSERT(mapped != nullptr);
	memcpy(mapped, buff, size);
	glUnmapBuffer(target);
}

} // end namespace anki