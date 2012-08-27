#include "anki/math/MathCommonSrc.h"

namespace anki {

//==============================================================================
// Constructors                                                                =
//==============================================================================

// default
inline Vec4::Vec4()
{
#if defined(ANKI_MATH_INTEL_SIMD)
	mm = _mm_setzero_ps();
#else
	arr[0] = arr[1] = arr[2] = arr[3] = 0.0;
#endif
}

// F32
inline Vec4::Vec4(F32 f)
{
#if defined(ANKI_MATH_INTEL_SIMD)
	mm = _mm_set1_ps(f);
#else
	arr[0] = arr[1] = arr[2] = arr[3] = f;
#endif
}

// F32[]
inline Vec4::Vec4(const F32 arr_[])
{
#if defined(ANKI_MATH_INTEL_SIMD)
	mm = _mm_load_ps(arr_);
#else
	arr[0] = arr_[0];
	arr[1] = arr_[1];
	arr[2] = arr_[2];
	arr[3] = arr_[3];
#endif
}

// F32, F32, F32, F32
inline Vec4::Vec4(const F32 x_, const F32 y_, const F32 z_,
	const F32 w_)
{
#if defined(ANKI_MATH_INTEL_SIMD)
	mm = _mm_set_ps(w_, z_, y_, x_);
#else
	x() = x_;
	y() = y_;
	z() = z_;
	w() = w_;
#endif
}

// vec2, F32, F32
inline Vec4::Vec4(const Vec2& v2, const F32 z_, const F32 w_)
{
	x() = v2.x();
	y() = v2.y();
	z() = z_;
	w() = w_;
}

// vec2, vec2
inline Vec4::Vec4(const Vec2& av2, const Vec2& bv2)
{
	x() = av2.x();
	y() = av2.y();
	z() = bv2.x();
	w() = bv2.y();
}

// vec3, F32
inline Vec4::Vec4(const Vec3& v3, const F32 w_)
{
	x() = v3.x();
	y() = v3.y();
	z() = v3.z();
	w() = w_;
}

// Copy
inline Vec4::Vec4(const Vec4& b)
{
#if defined(ANKI_MATH_INTEL_SIMD)
	mm = b.mm;
#else
	x() = b.x();
	y() = b.y();
	z() = b.z();
	w() = b.w();
#endif
}

// quat
inline Vec4::Vec4(const Quat& q)
{
	x() = q.x();
	y() = q.y();
	z() = q.z();
	w() = q.w();
}

// __m128
#if defined(ANKI_MATH_INTEL_SIMD)
inline Vec4::Vec4(const __m128& mm_)
{
	mm = mm_;
}
#endif

//==============================================================================
// Accessors                                                                   =
//==============================================================================

inline F32& Vec4::operator[](const U i)
{
	return arr[i];
}

inline F32 Vec4::operator[](const U i) const
{
	return arr[i];
}

inline F32& Vec4::x()
{
	return vec.x;
}

inline F32 Vec4::x() const
{
	return vec.x;
}

inline F32& Vec4::y()
{
	return vec.y;
}

inline F32 Vec4::y() const
{
	return vec.y;
}

inline F32& Vec4::z()
{
	return vec.z;
}

inline F32 Vec4::z() const
{
	return vec.z;
}

inline F32& Vec4::w()
{
	return vec.w;
}

inline F32 Vec4::w() const
{
	return vec.w;
}

#if defined(ANKI_MATH_INTEL_SIMD)
inline __m128& Vec4::getMm()
{
	return mm;
}


inline const __m128& Vec4::getMm() const
{
	return mm;
}
#endif

inline Vec2 Vec4::xy() const
{
	return Vec2(x(), y());
}

inline Vec3 Vec4::xyz() const
{
	return Vec3(x(), y(), z());
}

//==============================================================================
// Operators with same                                                         =
//==============================================================================

// =
inline Vec4& Vec4::operator=(const Vec4& b)
{
#if defined(ANKI_MATH_INTEL_SIMD)
	mm = b.mm;
#else
	x() = b.x();
	y() = b.y();
	z() = b.z();
	w() = b.w();
#endif
	return (*this);
}

// +
inline Vec4 Vec4::operator+(const Vec4& b) const
{
#if defined(ANKI_MATH_INTEL_SIMD)
	return Vec4(_mm_add_ps(mm, b.mm));
#else
	return Vec4(x() + b.x(), y() + b.y(), z() + b.z(), w() + b.w());
#endif
}

// +=
inline Vec4& Vec4::operator+=(const Vec4& b)
{
#if defined(ANKI_MATH_INTEL_SIMD)
	mm = _mm_add_ps(mm, b.mm);
#else
	x() += b.x();
	y() += b.y();
	z() += b.z();
	w() += b.w();
#endif
	return (*this);
}

// -
inline Vec4 Vec4::operator-(const Vec4& b) const
{
#if defined(ANKI_MATH_INTEL_SIMD)
	return Vec4(_mm_sub_ps(mm, b.mm));
#else
	return Vec4(x() - b.x(), y() - b.y(), z() - b.z(), w() - b.w());
#endif
}

// -=
inline Vec4& Vec4::operator-=(const Vec4& b)
{
#if defined(ANKI_MATH_INTEL_SIMD)
	mm = _mm_sub_ps(mm, b.mm);
#else
	x() -= b.x();
	y() -= b.y();
	z() -= b.z();
	w() -= b.w();
#endif
	return (*this);
}

// *
inline Vec4 Vec4::operator*(const Vec4& b) const
{
#if defined(ANKI_MATH_INTEL_SIMD)
	return Vec4(_mm_mul_ps(mm, b.mm));
#else
	return Vec4(x() * b.x(), y() * b.y(), z() * b.z(), w() * b.w());
#endif
}

// *=
inline Vec4& Vec4::operator*=(const Vec4& b)
{
#if defined(ANKI_MATH_INTEL_SIMD)
	mm = _mm_mul_ps(mm, b.mm);
#else
	x() *= b.x();
	y() *= b.y();
	z() *= b.z();
	w() *= b.w();
#endif
	return (*this);
}

// /
inline Vec4 Vec4::operator/(const Vec4& b) const
{
#if defined(ANKI_MATH_INTEL_SIMD)
	return Vec4(_mm_div_ps(mm, b.mm));
#else
	return Vec4(x() / b.x(), y() / b.y(), z() / b.z(), w() / b.w());
#endif
}

// /=
inline Vec4& Vec4::operator/=(const Vec4& b)
{
#if defined(ANKI_MATH_INTEL_SIMD)
	mm = _mm_div_ps(mm, b.mm);
#else
	x() /= b.x();
	y() /= b.y();
	z() /= b.z();
	w() /= b.w();
#endif
	return (*this);
}

// negative
inline Vec4 Vec4::operator-() const
{
	return Vec4(-x(), -y(), -z(), -w());
}

// ==
inline Bool Vec4::operator==(const Vec4& b) const
{
	Vec4 sub = (*this) - b;
	return Math::isZero(sub.x()) 
		&& Math::isZero(sub.y()) 
		&& Math::isZero(sub.z()) 
		&& Math::isZero(sub.w());
}

// !=
inline Bool Vec4::operator!=(const Vec4& b) const
{
	return !operator==(b);
}

// <
inline Bool Vec4::operator<(const Vec4& b) const
{
	return x() < b.x() && y() < b.y() && z() < b.z() && w() < b.w();
}

// <=
inline Bool Vec4::operator<=(const Vec4& b) const
{
	return x() <= b.x() && y() <= b.y() && z() <= b.z() && w() <= b.w();
}

//==============================================================================
// Operators with F32                                                        =
//==============================================================================

// Vec4 + F32
inline Vec4 Vec4::operator+(const F32 f) const
{
	return (*this) + Vec4(f);
}

// Vec4 += F32
inline Vec4& Vec4::operator+=(const F32 f)
{
	(*this) += Vec4(f);
	return (*this);
}

// Vec4 - F32
inline Vec4 Vec4::operator-(const F32 f) const
{
	return (*this) - Vec4(f);
}

// Vec4 -= F32
inline Vec4& Vec4::operator-=(const F32 f)
{
	(*this) -= Vec4(f);
	return (*this);
}

// Vec4 * F32
inline Vec4 Vec4::operator*(const F32 f) const
{
	return (*this) * Vec4(f);
}

// Vec4 *= F32
inline Vec4& Vec4::operator*=(const F32 f)
{
	(*this) *= Vec4(f);
	return (*this);
}

// Vec4 / F32
inline Vec4 Vec4::operator/(const F32 f) const
{
	return (*this) / Vec4(f);
}

// Vec4 /= F32
inline Vec4& Vec4::operator/=(const F32 f)
{
	(*this) /= Vec4(f);
	return (*this);
}

//==============================================================================
// Operators with other                                                        =
//==============================================================================

// Vec4 * mat4
inline Vec4 Vec4::operator*(const Mat4& m4) const
{
	return Vec4(
		x() * m4(0, 0) + y() * m4(1, 0) + z() * m4(2, 0) + w() * m4(3, 0),
		x() * m4(0, 1) + y() * m4(1, 1) + z() * m4(2, 1) + w() * m4(3, 1),
		x() * m4(0, 2) + y() * m4(1, 2) + z() * m4(2, 2) + w() * m4(3, 2),
		x() * m4(0, 3) + y() * m4(1, 3) + z() * m4(2, 3) + w() * m4(3, 3));
}

//==============================================================================
// Misc methods                                                                =
//==============================================================================

// dot
inline F32 Vec4::dot(const Vec4& b) const
{
#if defined(ANKI_MATH_INTEL_SIMD)
	F32 o;
	_mm_store_ss(&o, _mm_dp_ps(mm, b.mm, 0xF1));
	return o;
#else
	return x() * b.x() + y() * b.y() + z() * b.z() + w() * b.w();
#endif
}

// getLength
inline F32 Vec4::getLength() const
{
	return Math::sqrt(dot((*this)));
}

// getNormalized
inline Vec4 Vec4::getNormalized() const
{
	return (*this) / getLength();
}

// normalize
inline void Vec4::normalize()
{
	(*this) /= getLength();
}

//==============================================================================
// Friends                                                                     =
//==============================================================================

// F32 + Vec4
inline Vec4 operator+(const F32 f, const Vec4& v4)
{
	return v4 + f;
}

// F32 - Vec4
inline Vec4 operator-(const F32 f, const Vec4& v4)
{
	return Vec4(f) - v4;
}

// F32 * Vec4
inline Vec4 operator*(const F32 f, const Vec4& v4)
{
	return v4 * f;
}

// F32 / Vec4
inline Vec4 operator/(const F32 f, const Vec4& v4)
{
	return Vec4(f) / v4;
}

// Print
inline std::ostream& operator<<(std::ostream& s, const Vec4& v)
{
	s << v.x() << ' ' << v.y() << ' ' << v.z() << ' ' << v.w();
	return s;
}

} // end namespace
