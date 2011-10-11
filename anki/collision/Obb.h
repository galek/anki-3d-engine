#ifndef ANKI_COLLISION_OBB_H
#define ANKI_COLLISION_OBB_H

#include "anki/collision/CollisionShape.h"
#include "anki/math/Math.h"
#include <boost/array.hpp>


namespace anki {


/// @addtogroup Collision
/// @{

/// Object oriented bounding box
class Obb: public CollisionShape
{
	public:
		/// @name Constructors
		/// @{
		Obb()
		:	CollisionShape(CST_OBB)
		{}
		Obb(const Obb& b);
		Obb(const Vec3& center, const Mat3& rotation, const Vec3& extends);
		/// @}

		/// @name Accessors
		/// @{
		const Vec3& getCenter() const
		{
			return center;
		}
		Vec3& getCenter()
		{
			return center;
		}
		void setCenter(const Vec3& x)
		{
			center = x;
		}

		const Mat3& getRotation() const
		{
			return rotation;
		}
		Mat3& getRotation()
		{
			return rotation;
		}
		void setRotation(const Mat3& x)
		{
			rotation = x;
		}

		const Vec3& getExtend() const
		{
			return extends;
		}
		Vec3& getExtend()
		{
			return extends;
		}
		void setExtend(const Vec3& x)
		{
			extends = x;
		}
		/// @}

		/// @copydoc CollisionShape::accept
		void accept(Visitor& v)
		{
			v.visit(*this);
		}

		Obb getTransformed(const Transform& transform) const;

		/// Get a collision shape that includes this and the given. Its not
		/// very accurate
		Obb getCompoundShape(const Obb& b) const;

		/// @see CollisionShape::testPlane
		float testPlane(const Plane& plane) const;

		/// Calculate from a set of points
		template<typename Container>
		void set(const Container& container);

	public:
		/// @name Data
		/// @{
		Vec3 center;
		Mat3 rotation;
		/// With identity rotation this points to max (front, right, top in
		/// our case)
		Vec3 extends;
		/// @}

		/// Get extreme points in 3D space
		void getExtremePoints(boost::array<Vec3, 8>& points) const;
};
/// @}


} // end namespace


#include "anki/collision/Obb.inl.h"


#endif
