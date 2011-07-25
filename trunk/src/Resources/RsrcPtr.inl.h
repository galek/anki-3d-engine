#include "RsrcPtr.h"
#include "Util/Exception.h"
#include "Util/Assert.h"


//==============================================================================
// Constructor                                                                 =
//==============================================================================
template<typename Type>
RsrcPtr<Type>::RsrcPtr(const RsrcPtr& a):
	hook(a.hook)
{
	if(hook)
	{
		++hook->referenceCounter;
	}
}


//==============================================================================
// operator*                                                                   =
//==============================================================================
template<typename Type>
Type& RsrcPtr<Type>::operator*() const
{
	ASSERT(hook != NULL);
	return *hook->resource;
}


//==============================================================================
// operator->                                                                  =
//==============================================================================
template<typename Type>
Type* RsrcPtr<Type>::operator->() const
{
	ASSERT(hook != NULL);
	return hook->resource;
}


//==============================================================================
// loadRsrc                                                                    =
//==============================================================================
template<typename Type>
void RsrcPtr<Type>::loadRsrc(const char* filename)
{
	ASSERT(hook == NULL);
	hook = &ResourceManagerSingleton::getInstance().load<Type>(filename);
}


//==============================================================================
// unload                                                                      =
//==============================================================================
template<typename Type>
void RsrcPtr<Type>::unload()
{
	if(hook != NULL)
	{
		ResourceManagerSingleton::getInstance().unload<Type>(*hook);
		hook = NULL;
	}
}


//==============================================================================
// getRsrcName                                                                 =
//==============================================================================
template<typename Type>
const std::string& RsrcPtr<Type>::getRsrcName() const
{
	ASSERT(hook != NULL);
	return hook->uuid;
}