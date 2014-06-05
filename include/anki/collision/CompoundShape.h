// Copyright (C) 2014, Panagiotis Christopoulos Charitos.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

#ifndef ANKI_COLLISION_COMPOUND_SHAPE_H
#define ANKI_COLLISION_COMPOUND_SHAPE_H

#include "anki/collision/CollisionShape.h"
#include "anki/util/NonCopyable.h"

namespace anki {

/// @addtogroup collision
/// @{

/// A shape that contains other shapes
class CompoundShape: public CollisionShape, public NonCopyable
{
public:
	/// @name Constructors
	/// @{
	CompoundShape();
	/// @}

	/// Implements CollisionShape::testPlane
	F32 testPlane(const Plane& p) const;

	/// Implements CollisionShape::transform
	void transform(const Transform& trf);

	/// Implements CollisionShape::computeAabb
	void computeAabb(Aabb& b) const;

	/// Implements CollisionShape::accept
	void accept(MutableVisitor& v);

	/// Implements CollisionShape::accept
	void accept(ConstVisitor& v) const;

	/// The compound shape will not take ownership of the object
	void addShape(CollisionShape* shape);

private:
	static const U SHAPES_PER_CHUNK_COUNT = 8;

	class Chunk
	{
	public:
		Array<CollisionShape*, SHAPES_PER_CHUNK_COUNT> m_shapes;
		Chunk* m_next = nullptr;
	};

	Chunk m_dflt;

	template<typename TFunc>
	void iterateShapes(TFunc f) const
	{
		U count = 0;
		const Chunk* chunk = &m_dflt;

		do
		{
			U idx = SHAPES_PER_CHUNK_COUNT;
			while(idx-- != 0)
			{
				if(chunk->m_shapes[idx])
				{
					f(*const_cast<CollisionShape*>(chunk->m_shapes[idx]));
					++count;
				}
			}

			chunk = chunk->m_next;
		} while(chunk);
	
		ANKI_ASSERT(count > 0 && "Empty CompoundShape");
		(void)count;
	}
};

/// @}

} // end namespace anki

#endif

