/// @file
/// Contains misc functions

#ifndef ANKI_UTIL_FUNCTIONS_H
#define ANKI_UTIL_FUNCTIONS_H

#include "anki/util/StdTypes.h"
#include <string>

namespace anki {

/// @addtogroup util
/// @{
/// @addtogroup misc
/// @{

/// Pick a random number from min to max
extern I32 randRange(I32 min, I32 max);

/// Pick a random number from min to max
extern U32 randRange(U32 min, U32 max);

/// Pick a random number from min to max
extern F32 randRange(F32 min, F32 max);

/// Pick a random number from min to max
extern F64 randRange(F64 min, F64 max);

extern F32 randFloat(F32 max);

/// Get the size in bytes of a vector
template<typename Vec>
inline PtrSize getVectorSizeInBytes(const Vec& v)
{
	return v.size() * sizeof(typename Vec::value_type);
}

/// Trim a string
inline std::string trimString(const std::string& str, const char* what = " ")
{
	std::string out = str;
	out.erase(0, out.find_first_not_of(what));
	out.erase(out.find_last_not_of(what) + 1);
	return out;
}

/// Replace substring. Substitute occurances of @a from into @a to inside the
/// @a str string
extern std::string replaceAllString(const std::string& str, 
	const std::string& from, const std::string& to);

/// Delete a pointer properly 
template<typename T>
inline void propperDelete(T*& x)
{
	typedef char TypeMustBeComplete[sizeof(T) ? 1 : -1];
  	(void) sizeof(TypeMustBeComplete);
	delete x;
	x = nullptr;
}

/// A simple template trick to remove the pointer from one type
///
/// Example:
/// @code
/// double a = 1234.456;
/// RemovePointer<decltype(&a)>::Type b = a;
/// @endcode
/// The b is of type double
template<typename T>
struct RemovePointer;

template<typename T>
struct RemovePointer<T*>
{
	typedef T Type;
};

/// @}
/// @}

} // end namespace anki

#endif
