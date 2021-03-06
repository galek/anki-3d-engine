// Copyright (C) 2014, Panagiotis Christopoulos Charitos.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

#include "anki/scene/Octree.h"
#include "anki/scene/SpatialComponent.h"
#include "anki/scene/FrustumComponent.h"
#include "anki/scene/Light.h"
#include "anki/scene/Sector.h"
#include "anki/scene/SceneGraph.h"
#include "anki/util/Exception.h"
#include "anki/core/Logger.h"
#include "anki/scene/RenderComponent.h"

namespace anki {

#if 0

//==============================================================================
// OctreeNode                                                                  =
//==============================================================================

//==============================================================================
OctreeNode::OctreeNode(const Aabb& aabb_, OctreeNode* parent_,
	const SceneAllocator<U8>& alloc)
	: parent(parent_), aabb(aabb_), sceneNodes(alloc)
{
	for(OctreeNode* child : children)
	{
		child = nullptr;
	}
}

//==============================================================================
OctreeNode::~OctreeNode()
{
	ANKI_ASSERT(sceneNodes.size() == 0 
		&& "Deleting an OctreeNode with scene nodes");

	SceneAllocator<U8> alloc = sceneNodes.get_allocator();

	for(OctreeNode* onode : children)
	{
		if(onode)
		{
			ANKI_DELETE(onode, alloc);
		}
	}
}

//==============================================================================
void OctreeNode::addSceneNode(SceneNode* sn)
{
	ANKI_ASSERT(sn != nullptr);

	SpatialComponent* sp = sn->getSpatial();
	ANKI_ASSERT(sp != nullptr);

	if(this == sp->octreeNode)
	{
		// Nothing changes. Nothing to do
	}
	else
	{
		// Remove from current node ...
		if(sp->octreeNode != nullptr)
		{
			sp->octreeNode->removeSceneNode(sn);
		}

		// ... and add to a new
#if ANKI_CFG_OCTREE_THREAD_SAFE
		mtx.lock();
#endif
		sceneNodes.push_back(sn);
#if ANKI_CFG_OCTREE_THREAD_SAFE
		mtx.unlock();
#endif
		sp->octreeNode = this;
	}
}

//==============================================================================
void OctreeNode::removeSceneNode(SceneNode* sn)
{
	ANKI_ASSERT(sn != nullptr);

#if ANKI_CFG_OCTREE_THREAD_SAFE
	mtx.lock();
#endif

	SceneVector<SceneNode*>::iterator it =
		std::find(sceneNodes.begin(), sceneNodes.end(), sn);
	ANKI_ASSERT(it != sceneNodes.end());

	sceneNodes.erase(it);

#if ANKI_CFG_OCTREE_THREAD_SAFE
	mtx.unlock();
#endif

	sn->getSpatial()->octreeNode = nullptr;
}

//==============================================================================
void OctreeNode::addChild(U pos, OctreeNode* child)
{
	SceneAllocator<U8> alloc = sceneNodes.get_allocator();

	child->parent = this;

	if(children[pos])
	{
		OctreeNode* onode = children[pos];
		ANKI_DELETE(onode, alloc);
	}

	children[pos] = child;
}

//==============================================================================
const Octree& OctreeNode::getOctree() const
{
	const OctreeNode* root = this;
	while(root->parent != nullptr)
	{
		root = root->parent;
	}

	const RootOctreeNode* realRoot = static_cast<const RootOctreeNode*>(root);
	return *(realRoot->octree);
}

//==============================================================================
Octree& OctreeNode::getOctree()
{
	OctreeNode* root = this;
	while(root->parent != nullptr)
	{
		root = root->parent;
	}

	RootOctreeNode* realRoot = static_cast<RootOctreeNode*>(root);
	return *(realRoot->octree);
}

//==============================================================================
// Octree                                                                      =
//==============================================================================

//==============================================================================
Octree::Octree(Sector* sector_, const Aabb& aabb, U8 maxDepth_, F32 looseness_)
	:	sector(sector_),
		maxDepth(maxDepth_ < 1 ? 1 : maxDepth_), 
		looseness(looseness_),
		root(aabb, sector->getSectorGroup().getSceneGraph().getAllocator(), this)
{}

//==============================================================================
Octree::~Octree()
{}

//==============================================================================
void Octree::placeSceneNode(SceneNode* sn)
{
	SpatialComponent* sp = sn->getSpatial();
	ANKI_ASSERT(sp != nullptr);
	OctreeNode* toBePlacedNode = place(sp->getAabb());

	if(toBePlacedNode != nullptr)
	{
		toBePlacedNode->addSceneNode(sn);
	}
}

//==============================================================================
OctreeNode* Octree::place(const Aabb& aabb)
{
	if(aabb.collide(root.aabb))
	{
		// Run the recursive method
		return placeInternal(aabb, 0, root);
	}
	else
	{
		// Its completely outside the octree
		return nullptr;
	}
}

//==============================================================================
OctreeNode* Octree::placeInternal(const Aabb& aabb, U depth, OctreeNode& node)
{
	if(depth >= maxDepth)
	{
		return &node;
	}

	for(U i = 0; i < 2; ++i)
	{
		for(U j = 0; j < 2; ++j)
		{
			for(U k = 0; k < 2; ++k)
			{
				U id = i * 4 + j * 2 + k;

				// Get the node's AABB. If there is no node then calculate the
				// AABB
				Aabb childAabb;
				Bool child = node.children[id] != nullptr;
				if(child)
				{
					// There is a child
					childAabb = node.children[id]->aabb;
				}
				else
				{
					// Calculate
					calcAabb(i, j, k, node.aabb, childAabb);
				}

				// If aabb its completely inside the target => go deeper
				if(aabb.getMax() <= childAabb.getMax() 
					&& aabb.getMin() >= childAabb.getMin())
				{
					if(!child)
					{
#if ANKI_CFG_OCTREE_THREAD_SAFE
						node.mtx.lock();
					
						// Check again now that the mutex is locked
						volatile Bool checkAgain = node.children[id] == nullptr;
						if(checkAgain)
#endif
						{
							SceneAllocator<U8> alloc =
								sector->getSectorGroup().getSceneGraph().
								getAllocator();

							// Create new node if needed
							OctreeNode* newNode =
								ANKI_NEW(OctreeNode, alloc,
								childAabb, &node, alloc);

							node.addChild(id, newNode);
						}
#if ANKI_CFG_OCTREE_THREAD_SAFE
						node.mtx.unlock();
#endif
					}

					ANKI_ASSERT(node.children[id]);
					return placeInternal(aabb, depth + 1, *node.children[id]);
				}
			} // k
		} // j
	} // i

	return &node;
}

//==============================================================================
void Octree::calcAabb(U i, U j, U k, const Aabb& paabb, Aabb& out) const
{
	const Vec3& min = paabb.getMin();
	const Vec3& max = paabb.getMax();
	Vec3 d = (max - min) / 2.0;

	Vec3 omin;
	omin.x() = min.x() + d.x() * i;
	omin.y() = min.y() + d.y() * j;
	omin.z() = min.z() + d.z() * k;

	Vec3 omax = omin + d;

	// Scale the AABB with looseness
	F32 tmp0 = (1.0 + looseness) / 2.0;
	F32 tmp1 = (1.0 - looseness) / 2.0;
	Vec3 nomin = tmp0 * omin + tmp1 * omax;
	Vec3 nomax = tmp0 * omax + tmp1 * omin;

	// Crop to fit the parent's AABB
	for(U n = 0; n < 3; ++n)
	{
		if(nomin[n] < min[n])
		{
			nomin[n] = min[n];
		}

		if(nomax[n] > max[n])
		{
			nomax[n] = max[n];
		}
	}

	out = Aabb(nomin, nomax);
}

//==============================================================================
void Octree::doVisibilityTests(SceneNode& fsn, VisibilityTest test,
	VisibilityTestResults& visibles)
{
	ANKI_ASSERT(fsn.getFrustumable());
	doVisibilityTestsInternal(fsn, test, visibles, root);
}

//==============================================================================
void Octree::doVisibilityTestsInternal(SceneNode& fsn,
	VisibilityTest test, VisibilityTestResults& visible, OctreeNode& node)
{
	const Frustumable& fr = *fsn.getFrustumable();

	// Put node's scene nodes
	for(SceneNode* sn : node.sceneNodes)
	{
		SpatialComponent* sp = sn->getSpatial();
		ANKI_ASSERT(sp);

		if(fr.insideFrustum(*sp))
		{
			Renderable* r = sn->getRenderable();
			if(r != nullptr && (test & VT_RENDERABLES))
			{
				if(!((test & VT_ONLY_SHADOW_CASTERS) == true
					&& !r->getRenderableMaterial().getShadow()))
				{
					//visible.renderables.push_back(VisibleNode(sn, 0));
				}
			}

			Light* l = sn->getLight();
			if(l != nullptr && (test & VT_LIGHTS))
			{
				//visible.lights.push_back(VisibleNode(sn, 0));
			}
		}
	}

	// Go to children
	for(OctreeNode* onode : node.children)
	{
		if(onode != nullptr && fr.insideFrustum(onode->getAabb()))
		{
			doVisibilityTestsInternal(fsn, test, visible, *onode);
		}
	}
}

#endif

} // end namespace anki
