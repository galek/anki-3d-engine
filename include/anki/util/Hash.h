// Copyright (C) 2014, Panagiotis Christopoulos Charitos.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

#include "anki/util/StdTypes.h"

namespace anki {

/// @addtogroup util_other
/// @{

/// Computes a hash of a buffer.
/// This function implements the MurmurHash2 algorithm by Austin Appleby.
/// @param[in] buffer The buffer to hash.
/// @param bufferSize The size of the buffer.
/// @param seed A unique seed.
/// @return The hash.
U64 computeHash(const void* buffer, U32 bufferSize, U64 seed = 123);

/// @}

} // end namespace anki

