// Copyright (C) 2014, Panagiotis Christopoulos Charitos.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

#include "anki/math/CommonSrc.h"

namespace anki {

//==============================================================================
// Friends                                                                     =
//==============================================================================

//==============================================================================
/// @memberof TVec4
template<typename T>
TVec4<T> operator+(const T f, const TVec4<T>& v4)
{
	return v4 + f;
}

//==============================================================================
/// @memberof TVec4
template<typename T>
TVec4<T> operator-(const T f, const TVec4<T>& v4)
{
	return TVec4<T>(f) - v4;
}

//==============================================================================
/// @memberof TVec4
template<typename T>
TVec4<T> operator*(const T f, const TVec4<T>& v4)
{
	return v4 * f;
}

//==============================================================================
/// @memberof TVec4
template<typename T>
TVec4<T> operator/(const T f, const TVec4<T>& v4)
{
	return TVec4<T>(f) / v4;
}

#if ANKI_SIMD == ANKI_SIMD_SSE

//==============================================================================
// SSE specializations                                                         =
//==============================================================================

//==============================================================================
// Constructors                                                                =
//==============================================================================

//==============================================================================
template<>
inline TVec4<F32>::TVec4(F32 f)
{
	m_simd = _mm_set1_ps(f);
}

//==============================================================================
template<>
inline TVec4<F32>::TVec4(const F32 arr[])
{
	m_simd = _mm_load_ps(arr);
}

//==============================================================================
template<>
inline TVec4<F32>::TVec4(const F32 x_, const F32 y_, const F32 z_, 
	const F32 w_)
{
	m_simd = _mm_set_ps(w_, z_, y_, x_);
}

//==============================================================================
template<>
inline TVec4<F32>::TVec4(const TVec4<F32>& b)
	: Base()
{
	m_simd = b.m_simd;
}

//==============================================================================
// Operators with same                                                         =
//==============================================================================

//==============================================================================
template<>
inline TVec4<F32>& TVec4<F32>::Base::operator=(const TVec4<F32>& b)
{
	m_simd = b.m_simd;
	return static_cast<TVec4<F32>&>(*this);
}

//==============================================================================
template<>
inline TVec4<F32> TVec4<F32>::Base::operator+(const TVec4<F32>& b) const
{
	return TVec4<F32>(_mm_add_ps(m_simd, b.m_simd));
}

//==============================================================================
template<>
inline TVec4<F32>& TVec4<F32>::Base::operator+=(const TVec4<F32>& b)
{
	m_simd = _mm_add_ps(m_simd, b.m_simd);
	return static_cast<TVec4<F32>&>(*this);
}

//==============================================================================
template<>
inline TVec4<F32> TVec4<F32>::Base::operator-(const TVec4<F32>& b) const
{
	return TVec4<F32>(_mm_sub_ps(m_simd, b.m_simd));
}

//==============================================================================
template<>
inline TVec4<F32>& TVec4<F32>::Base::operator-=(const TVec4<F32>& b)
{
	m_simd = _mm_sub_ps(m_simd, b.m_simd);
	return static_cast<TVec4<F32>&>(*this);
}

//==============================================================================
template<>
inline TVec4<F32> TVec4<F32>::Base::operator*(const TVec4<F32>& b) const
{
	return TVec4<F32>(_mm_mul_ps(m_simd, b.m_simd));
}

//==============================================================================
template<>
inline TVec4<F32>& TVec4<F32>::Base::operator*=(const TVec4<F32>& b)
{
	m_simd = _mm_mul_ps(m_simd, b.m_simd);
	return static_cast<TVec4<F32>&>(*this);
}

//==============================================================================
template<>
inline TVec4<F32> TVec4<F32>::Base::operator/(const TVec4<F32>& b) const
{
	return TVec4<F32>(_mm_div_ps(m_simd, b.m_simd));
}

//==============================================================================
template<>
inline TVec4<F32>& TVec4<F32>::Base::operator/=(const TVec4<F32>& b)
{
	m_simd = _mm_div_ps(m_simd, b.m_simd);
	return static_cast<TVec4<F32>&>(*this);
}

//==============================================================================
template<>
inline TVec4<F32> TVec4<F32>::cross(const TVec4<F32>& b) const
{
	ANKI_ASSERT(isZero<F32>(Base::w()));
	ANKI_ASSERT(isZero<F32>(b.w()));

	const auto& a = *this;
	const auto mask0 = _MM_SHUFFLE(3, 0, 2, 1);
	const auto mask1 = _MM_SHUFFLE(3, 1, 0, 2);

	__m128 tmp0 = _mm_mul_ps(_mm_shuffle_ps(a.m_simd, a.m_simd, mask0), 
		_mm_shuffle_ps(b.m_simd, b.m_simd, mask1));
	__m128 tmp1 = _mm_mul_ps(_mm_shuffle_ps(a.m_simd, a.m_simd, mask0), 
		_mm_shuffle_ps(b.m_simd, b.m_simd, mask1));

	return TVec4<F32>(_mm_sub_ps(tmp0, tmp1));	
}

//==============================================================================
// Other                                                                       =
//==============================================================================

//==============================================================================
template<>
inline F32 TVec4<F32>::Base::dot(const TVec4<F32>& b) const
{
	F32 o;
	_mm_store_ss(&o, _mm_dp_ps(m_simd, b.m_simd, 0xF1));
	return o;
}

//==============================================================================
template<>
inline TVec4<F32> TVec4<F32>::Base::getNormalized() const
{
	__m128 inverse_norm = _mm_rsqrt_ps(_mm_dp_ps(m_simd, m_simd, 0xFF));
	return TVec4<F32>(_mm_mul_ps(m_simd, inverse_norm));
}

//==============================================================================
template<>
inline void TVec4<F32>::Base::normalize()
{
	__m128 inverseNorm = _mm_rsqrt_ps(_mm_dp_ps(m_simd, m_simd, 0xFF));
	m_simd = _mm_mul_ps(m_simd, inverseNorm);
}

#elif ANKI_SIMD == ANKI_SIMD_NEON

//==============================================================================
// NEON specializations                                                        =
//==============================================================================

#	error "TODO"

#endif

} // end namespace anki
