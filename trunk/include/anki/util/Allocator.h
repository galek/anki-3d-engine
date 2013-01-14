#ifndef ANKI_UTIL_ALLOCATOR_H
#define ANKI_UTIL_ALLOCATOR_H

#include "anki/util/Exception.h"
#include "anki/util/Assert.h"
#include <cstddef> // For ptrdiff_t
#include <cstring> // For memset
#include <atomic>

#define ANKI_DEBUG_ALLOCATORS ANKI_DEBUG

namespace anki {

namespace detail {

/// Static methods for the #Allocator class
struct AllocatorStatic
{
	static PtrSize allocatedSize;

	/// Print a few debugging messages
	static void dump();

	/// Allocate memory
	static void* malloc(PtrSize size);

	/// Free memory
	static void free(void* p, PtrSize size);
};

} // end namespace detail

/// The default allocator. It uses malloc and free for 
/// allocations/deallocations. It's STL compatible
template<typename T>
class Allocator
{
public:
	// Typedefs
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T value_type;

	/// Default constructor
	Allocator() throw()
	{}
	/// Copy constructor
	Allocator(const Allocator&) throw()
	{}
	/// Copy constructor with another type
	template<typename U>
	Allocator(const Allocator<U>&) throw()
	{}

	/// Destructor
	~Allocator()
	{}

	/// Copy
	Allocator<T>& operator=(const Allocator&)
	{
		return *this;
	}
	/// Copy with another type
	template<typename U>
	Allocator& operator=(const Allocator<U>&) 
	{
		return *this;
	}

	/// Get address of reference
	pointer address(reference x) const 
	{
		return &x; 
	}
	/// Get const address of const reference
	const_pointer address(const_reference x) const 
	{
		return &x;
	}

	/// Allocate memory
	pointer allocate(size_type n, const void* = 0)
	{
		size_type size = n * sizeof(value_type);
		return (pointer)AllocatorStatic::malloc(size);
	}

	/// Deallocate memory
	void deallocate(void* p, size_type n)
	{
		size_type size = n * sizeof(T);
		AllocatorStatic::free(p, size);
	}

	/// Call constructor
	void construct(pointer p, const T& val)
	{
		// Placement new
		new ((T*)p) T(val); 
	}
	/// Call constructor with more arguments
	template<typename U, typename... Args>
	void construct(U* p, Args&&... args)
	{
		// Placement new
		::new((void*)p) U(std::forward<Args>(args)...);
	}

	/// Call the destructor of p
	void destroy(pointer p) 
	{
		p->~T();
	}
	/// Call the destructor of p of type U
	template<typename U>
	void destroy(U* p)
	{
		p->~U();
	}

	/// Get the max allocation size
	size_type max_size() const 
	{
		return size_type(-1); 
	}

	/// A struct to rebind the allocator to another allocator of type U
	template<typename U>
	struct rebind
	{ 
		typedef Allocator<U> other; 
	};
};

/// Another allocator of the same type can deallocate from this one
template<typename T1, typename T2>
inline bool operator==(const Allocator<T1>&, const Allocator<T2>&)
{
	return true;
}

/// Another allocator of the another type cannot deallocate from this one
template<typename T1, typename AnotherAllocator>
inline bool operator==(const Allocator<T1>&, const AnotherAllocator&)
{
	return false;
}

} // end namespace anki

#endif
