// Copyright (C) 2014, Panagiotis Christopoulos Charitos.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

#ifndef ANKI_UTIL_EXCEPTION_H
#define ANKI_UTIL_EXCEPTION_H

#include "anki/Config.h"
#include "anki/util/StdTypes.h"
#include <exception>
#include <utility>

namespace anki {

/// @addtogroup util_other
/// @{

/// Mother of all AnKi exceptions.
///
/// Custom exception that takes file, line and function that throw it. Throw 
/// it using the ANKI_EXCEPTION macro
class Exception: public std::exception
{
public:
	/// Constructor
	explicit Exception(const CString& file, I line, const CString& func, 
		const CString& errorFmt, ...) noexcept;

	/// Copy constructor
	Exception(const Exception& e) noexcept;

	/// Move constructor
	Exception(Exception&& e) noexcept
	{
		*this = std::move(e);
	}

	/// Destructor. Do nothing
	~Exception() noexcept;

	/// Move
	Exception& operator=(Exception&& b) noexcept;

	/// For re-throws.
	/// Usage:
	/// @code
	/// catch(std::exception& e) {
	/// 	throw Exception("message", ...) << e;
	/// }
	/// @endcode
	Exception operator<<(const std::exception& e) const;

	/// Implements std::exception::what()
	const char* what() const noexcept
	{
		return m_err;
	}

private:
	char* m_err;

	Exception() noexcept
	:	m_err(nullptr)
	{}

	/// Synthesize the error string
	static CString synthErr(const CString& error, const CString& file,
		I line, const CString& func) noexcept;
};

/// Macro for easy throwing
#define ANKI_EXCEPTION(...) \
	Exception(CString(ANKI_FILE), __LINE__, CString(ANKI_FUNC), __VA_ARGS__)

/// @}

} // end namespace anki

#endif
