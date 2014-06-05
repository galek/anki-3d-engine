// Copyright (C) 2014, Panagiotis Christopoulos Charitos.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

#ifndef ANKI_MATH_AXISANG_H
#define ANKI_MATH_AXISANG_H

#include "anki/math/CommonIncludes.h"

namespace anki {

/// @addtogroup math
/// @{

/// Axis angles. Used for rotations
template<typename T>
class TAxisang
{
public:
	/// @name Constructors
	/// @{
	explicit TAxisang()
		: m_ang(0.0), m_axis(0.0)
	{}
	
	TAxisang(const TAxisang& b)
		: m_ang(b.ang), m_axis(b.axis)
	{}

	explicit TAxisang(const T rad, const TVec3<T>& axis)
		: m_ang(rad), m_axis(axis)
	{}

	explicit TAxisang(const TQuat<T>& q)
	{
		m_ang = 2.0 * acos<T>(q.w());
		T length = sqrt<T>(1.0 - q.w() * q.w());
		if(!isZero<T>(length))
		{
			length = 1.0 / length;
			m_axis = TVec3<T>(q.x() * length, q.y() * length, q.z() * length);
		}
		else
		{
			m_axis = TVec3<T>(0.0);
		}
	}

	explicit TAxisang(const TMat3<T>& m3)
	{
		if(isZero<T>(m3(0, 1) - m3(1, 0))
			&& isZero<T>(m3(0, 2) - m3(2, 0)) 
			&& isZero<T>(m3(1, 2) - m3(2, 1)))
		{

			if((fabs<T>(m3(0, 1) + m3(1, 0)) < 0.1) 
				&& (fabs<T>(m3(0, 2) + m3(2, 0)) < 0.1) 
				&& (fabs<T>(m3(1, 2) + m3(2, 1)) < 0.1) 
				&& (fabs<T>(m3(0, 0) + m3(1, 1) + m3(2, 2)) - 3) < 0.1)
			{
				m_axis = TVec3<T>(1.0, 0.0, 0.0);
				m_ang = 0.0;
				return;
			}

			m_ang = getPi<T>();
			m_axis.x() = (m3(0, 0) + 1.0) / 2.0;
			if(m_axis.x() > 0.0)
			{
				m_axis.x() = sqrt(m_axis.x());
			}
			else
			{
				m_axis.x() = 0.0;
			}
			m_axis.y() = (m3(1, 1) + 1.0) / 2.0;
			if(m_axis.y() > 0.0)
			{
				m_axis.y() = sqrt(m_axis.y());
			}
			else
			{
				m_axis.y() = 0.0;
			}

			m_axis.z() = (m3(2, 2) + 1.0) / 2.0;
			if(m_axis.z() > 0.0)
			{
				m_axis.z() = sqrt(m_axis.z());
			}
			else
			{
				m_axis.z() = 0.0;
			}

			Bool xZero = isZero<T>(m_axis.x());
			Bool yZero = isZero<T>(m_axis.y());
			Bool zZero = isZero<T>(m_axis.z());
			Bool xyPositive = (m3(0, 1) > 0);
			Bool xzPositive = (m3(0, 2) > 0);
			Bool yzPositive = (m3(1, 2) > 0);
			if(xZero && !yZero && !zZero)
			{
				if(!yzPositive)
				{
					m_axis.y() = -m_axis.y();
				}
			}
			else if(yZero && !zZero)
			{
				if(!xzPositive)
				{
					m_axis.z() = -m_axis.z();
				}
			}
			else if(zZero)
			{
				if(!xyPositive)
				{
					m_axis.x() = -m_axis.x();
				}
			}

			return;
		}

		T s = sqrt((m3(2, 1) - m3(1, 2)) * (m3(2, 1) - m3(1, 2)) 
			+ (m3(0, 2) - m3(2, 0)) * (m3(0, 2) - m3(2, 0)) 
			+ (m3(1, 0) - m3(0, 1)) * (m3(1, 0) - m3(0, 1)));

		if(fabs(s) < 0.001)
		{
			s = 1.0;
		}

		m_ang = acos<T>((m3(0, 0) + m3(1, 1) + m3(2, 2) - 1.0) / 2.0);
		m_axis.x() = (m3(2, 1) - m3(1, 2)) / s;
		m_axis.y() = (m3(0, 2) - m3(2, 0)) / s;
		m_axis.z() = (m3(1, 0) - m3(0, 1)) / s;		
	}
	/// @}

	/// @name Accessors
	/// @{
	T getAngle() const
	{
		return m_ang;
	}

	T& getAngle()
	{
		return m_ang;
	}

	void setAngle(const T a)
	{
		m_ang = a;
	}

	const TVec3<T>& getAxis() const
	{
		return m_axis;
	}

	TVec3<T>& getAxis()
	{
		return m_axis;
	}

	void setAxis(const TVec3<T>& a)
	{
		m_axis = a;
	}
	/// @}

	/// @name Operators with same type
	/// @{
	TAxisang& operator=(const TAxisang& b)
	{
		m_ang = b.m_ang;
		m_axis = b.m_axis;
		return *this;
	}
	/// @}

	/// @name Other
	/// @{
	std::string toString() const
	{
		std::string s = "axis: " + m_axis.toString() 
			+ ", angle: " + std::to_string(m_ang);
		return s;
	}
	/// @}

private:
	/// @name Data
	/// @{
	T m_ang;
	TVec3<T> m_axis;
	/// @}
};

/// F32 Axisang
typedef TAxisang<F32> Axisang;
/// @}

} // end namespace anki

#endif
