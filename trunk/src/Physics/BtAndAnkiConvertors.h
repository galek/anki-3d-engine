#ifndef PHYCONVERTORS_H
#define PHYCONVERTORS_H

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include "Common.h"
#include "Math.h"


inline Vec3 toAnki(const btVector3& v)
{
	return Vec3(v.x(), v.y(), v.z());
}


inline Vec4 toAnki(const btVector4& v)
{
	return Vec4(v.x(), v.y(), v.z(), v.w());
}


inline Mat3 toAnki(const btMatrix3x3& m)
{
	Mat3 m3;
	m3.setRows(toAnki(m[0]), toAnki(m[1]), toAnki(m[2]));
	return m3;
}


inline Quat toAnki(const btQuaternion& q)
{
	return Quat(q.x(), q.y(), q.z(), q.w());
}


inline Transform toAnki(const btTransform& t)
{
	Transform out;
	out.rotation = toAnki(t.getBasis());
	out.origin = toAnki(t.getOrigin());
	out.scale = 1.0;
	return out;
}


inline btVector3 toBt(const Vec3& v)
{
	return btVector3(v.x,  v.y, v.z);
}


inline btVector4 toBt(const Vec4& v)
{
	return btVector4(v.x,  v.y, v.z, v.w);
}


inline btMatrix3x3 toBt(const Mat3 m)
{
	btMatrix3x3 r;
	r[0] = toBt(m.getRow(0));
	r[1] = toBt(m.getRow(1));
	r[2] = toBt(m.getRow(2));
	return r;
}


inline btTransform toBt(const Mat4& m)
{
	btTransform r;
	r.setFromOpenGLMatrix(&(m.getTransposed())[0]);
	return r;
}


inline btQuaternion toBt(const Quat& q)
{
	return btQuaternion(q.x, q.y, q.z, q.w);
}


inline btTransform toBt(const Transform& trf)
{
	btTransform r;
	r.setOrigin(toBt(trf.origin));
	r.setRotation(toBt(Quat(trf.rotation)));
	return r;
}


#endif
