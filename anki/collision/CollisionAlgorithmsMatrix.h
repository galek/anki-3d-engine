#ifndef ANKI_COLLISION_COLLISION_ALGORITHMS_MATRIX_H
#define ANKI_COLLISION_COLLISION_ALGORITHMS_MATRIX_H

#include "anki/collision/Forward.h"


namespace anki {


/// @addtogroup Collision
/// @{
///
/// Provides the collision algorithms that detect collision between collision
/// shapes
///
/// +------+------+------+------+------+------+------+------+
/// |      | LS   | OBB  | PCC  | P    | R    | S    | AABB |
/// +------+------+------+------+------+------+------+------+
/// | LS   |      |      |      |      |      |      |      |
/// +------+------+------+------+------+------+------+------+
/// | OBB  |      |      |      |      |      |      |      |
/// +------+------+------+------+------+------+------+------+
/// | PCS  |      |      |      |      |      |      |      |
/// +------+------+------+------+------+------+------+------+
/// | P    |      |      |      |      |      |      |      |
/// +------+------+------+------+------+------+------+------+
/// | R    |      |      |      |      |      |      |      |
/// +------+------+------+------+------+------+------+------+
/// | S    |      |      |      |      |      |      |      |
/// +------+------+------+------+------+------+------+------+
/// | AABB |      |      |      |      |      |      |      |
/// +------+------+------+------+------+------+------+------+
class CollisionAlgorithmsMatrix
{
	public:
		typedef PerspectiveCameraShape Pcs;
		typedef LineSegment Ls;

		/// Generic collide function. It doesn't uses visitor pattern for
		/// speed reasons
		static bool collide(const CollisionShape& a, const CollisionShape& b);

		// 1st line (LS)
		static bool collide(const Ls& a, const Ls& b);
		static bool collide(const Ls& a, const Obb& b);
		static bool collide(const Ls& a, const Pcs& b);
		static bool collide(const Ls& a, const Plane& b);
		static bool collide(const Ls& a, const Ray& b);
		static bool collide(const Ls& a, const Sphere& b);
		static bool collide(const Ls& a, const Aabb& b);

		// 2nd line (OBB)
		static bool collide(const Obb& a, const Ls& b)
		{
			return collide(b, a);
		}
		static bool collide(const Obb& a, const Obb& b);
		static bool collide(const Obb& a, const Pcs& b);
		static bool collide(const Obb& a, const Plane& b);
		static bool collide(const Obb& a, const Ray& b);
		static bool collide(const Obb& a, const Sphere& b);
		static bool collide(const Obb& a, const Aabb& b);

		// 3rd line (PCS)
		static bool collide(const Pcs& a, const Ls& b)
		{
			return collide(b, a);
		}
		static bool collide(const Pcs& a, const Obb& b)
		{
			return collide(b, a);
		}
		static bool collide(const Pcs& a, const Pcs& b);
		static bool collide(const Pcs& a, const Plane& b);
		static bool collide(const Pcs& a, const Ray& b);
		static bool collide(const Pcs& a, const Sphere& b);
		static bool collide(const Pcs& a, const Aabb& b);

		// 4th line (P)
		static bool collide(const Plane& a, const Ls& b)
		{
			return collide(b, a);
		}
		static bool collide(const Plane& a, const Obb& b)
		{
			return collide(b, a);
		}
		static bool collide(const Plane& a,const Pcs& b)
		{
			return collide(b, a);
		}
		static bool collide(const Plane& a, const Plane& b);
		static bool collide(const Plane& a, const Ray& b);
		static bool collide(const Plane& a, const Sphere& b);
		static bool collide(const Plane& a, const Aabb& b);

		// 5th line (R)
		static bool collide(const Ray& a, const Ls& b)
		{
			return collide(b, a);
		}
		static bool collide(const Ray& a, const Obb& b)
		{
			return collide(b, a);
		}
		static bool collide(const Ray& a, const Pcs& b)
		{
			return collide(b, a);
		}
		static bool collide(const Ray& a, const Plane& b)
		{
			return collide(b, a);
		}
		static bool collide(const Ray& a, const Ray& b);
		static bool collide(const Ray& a, const Sphere& b);
		static bool collide(const Ray& a, const Aabb& b);

		// 6th line (S)
		static bool collide(const Sphere& a, const Ls& b)
		{
			return collide(b, a);
		}
		static bool collide(const Sphere& a, const Obb& b)
		{
			return collide(b, a);
		}
		static bool collide(const Sphere& a, const Pcs& b)
		{
			return collide(b, a);
		}
		static bool collide(const Sphere& a, const Plane& b)
		{
			return collide(b, a);
		}
		static bool collide(const Sphere& a, const Ray& b)
		{
			return collide(b, a);
		}
		static bool collide(const Sphere& a, const Sphere& b);
		static bool collide(const Sphere& a, const Aabb& b);

		// 7th line (AABB)
		static bool collide(const Aabb& a, const Ls& b)
		{
			return collide(b, a);
		}
		static bool collide(const Aabb& a, const Obb& b)
		{
			return collide(b, a);
		}
		static bool collide(const Aabb& a, const Pcs& b)
		{
			return collide(b, a);
		}
		static bool collide(const Aabb& a, const Plane& b)
		{
			return collide(b, a);
		}
		static bool collide(const Aabb& a, const Ray& b)
		{
			return collide(b, a);
		}
		static bool collide(const Aabb& a, const Sphere& b)
		{
			return collide(b, a);
		}
		static bool collide(const Aabb& a, const Aabb& b);

	private:
		template<typename T>
		static bool tcollide(const CollisionShape& a, const CollisionShape& b);
};
/// @}


} // end namespace


#endif
