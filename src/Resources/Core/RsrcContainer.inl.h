#include <boost/filesystem.hpp>
#include "RsrcContainer.h"


//======================================================================================================================
// Destructor                                                                                                          =
//======================================================================================================================
template<typename Type>
RsrcContainer<Type>::~RsrcContainer()
{
	DEBUG_ERR(BaseClass::size() != 0); // this means that somehow a resource is still loaded
}


//======================================================================================================================
// findByName                                                                                                          =
//======================================================================================================================
template<typename Type>
typename RsrcContainer<Type>::Iterator RsrcContainer<Type>::findByName(const char* name)
{
	Iterator it = BaseClass::begin();
	while(it != BaseClass::end())
	{
		if((*it).name == name)
			return it;
		++it;
	}

	return it;
}


//======================================================================================================================
// findByNameAndPath                                                                                                   =
//======================================================================================================================
template<typename Type>
typename RsrcContainer<Type>::Iterator RsrcContainer<Type>::findByNameAndPath(const char* name, const char* path)
{
	Iterator it = BaseClass::begin();
	while(it != BaseClass::end())
	{
		if((*it)->name == name && (*it)->path == path)
			return it;
		++it;
	}

	return it;
}


//======================================================================================================================
// findByPtr                                                                                                           =
//======================================================================================================================
template<typename Type>
typename RsrcContainer<Type>::Iterator RsrcContainer<Type>::findByPtr(Type* ptr)
{
	Iterator it = BaseClass::begin();
	while(it != BaseClass::end())
	{
		if(ptr == (*it)) return it;
		++it;
	}

	return it;
}



//======================================================================================================================
// load                                                                                                                =
//======================================================================================================================
template<typename Type>
Type* RsrcContainer<Type>::load(const char* fname)
{
	DEBUG_ERR(fname == NULL);
	filesystem::path fpathname = filesystem::path(fname);
	string name = fpathname.filename();
	string path = fpathname.parent_path().string();
	Iterator it = findByNameAndPath(name.c_str(), path.c_str());

	// if already loaded then inc the users and return the pointer
	if(it != BaseClass::end())
	{
		++ (*it)->referenceCounter;
		return (*it);
	}

	// else create new, loaded and update the container
	Type* newInstance = new Type();
	newInstance->name = name;
	newInstance->path = path;
	newInstance->referenceCounter = 1;

	if(!newInstance->load(fname))
	{
		ERROR("Cannot load \"" << fname << '\"');
		delete newInstance;
		return NULL;
	}
	BaseClass::push_back(newInstance);

	return newInstance;
}


//======================================================================================================================
// unload                                                                                                              =
//======================================================================================================================
template<typename Type>
void RsrcContainer<Type>::unload(Type* x)
{
	Iterator it = findByPtr(x);
	if(it == BaseClass::end())
	{
		ERROR("Cannot find resource with pointer 0x" << hex << x);
		return;
	}

	Type* del_ = (*it);
	DEBUG_ERR(del_->referenceCounter < 1); // WTF?

	--del_->referenceCounter;

	// if no other users then call unload and update the container
	if(del_->referenceCounter == 0)
	{
		delete del_;
		BaseClass::erase(it);
	}
}
