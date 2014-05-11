#include "anki/gl/GlClientBufferHandle.h"
#include "anki/gl/GlClientBuffer.h"
#include "anki/gl/GlJobChainHandle.h"
#include "anki/gl/GlManager.h"

namespace anki {

//==============================================================================
GlClientBufferHandle::GlClientBufferHandle()
{}

//==============================================================================
GlClientBufferHandle::GlClientBufferHandle(
	GlJobChainHandle& jobs, PtrSize size, void* preallocatedMem)
{
	ANKI_ASSERT(!isCreated());

	auto alloc = jobs._getAllocator();

	typedef GlHandleDefaultDeleter<
		GlClientBuffer, GlJobChainAllocator<GlClientBuffer>> Deleter;

	if(preallocatedMem != nullptr)
	{
		*static_cast<Base*>(this) = Base(
			nullptr, 
			alloc, 
			Deleter(),
			preallocatedMem, 
			size);
	}
	else
	{
		*static_cast<Base*>(this) = Base(
			nullptr, 
			alloc, 
			Deleter(),
			alloc, 
			size);
	}
}

//==============================================================================
GlClientBufferHandle::~GlClientBufferHandle()
{}

//==============================================================================
void* GlClientBufferHandle::getBaseAddress()
{
	return _get().getBaseAddress();
}

//==============================================================================
PtrSize GlClientBufferHandle::getSize() const
{
	return _get().getSize();
}

} // end namespace anki
