#include "anki/scene/Visibility.h"
#include "anki/scene/Scene.h"
#include "anki/scene/Frustumable.h"
#include "anki/scene/Light.h"
#include "anki/renderer/Renderer.h"

namespace anki {

//==============================================================================
struct VisibilityTestJob: ThreadJob
{
	U nodesCount = 0;
	Scene::Types<SceneNode>::Container::iterator nodes;
	Frustumable* frustumable = nullptr;
	Renderer* renderer = nullptr;
	SceneAllocator<U8> frameAlloc;

	VisibilityTestResults* visible;

	/// Do the tests
	void operator()(U threadId, U threadsCount)
	{
		U64 start, end;
		choseStartEnd(threadId, threadsCount, nodesCount, start, end);

		visible = ANKI_NEW(VisibilityTestResults, frameAlloc, frameAlloc);

		for(auto it = nodes + start; it != nodes + end; it++)
		{
			SceneNode* node = *it;

			Frustumable* fr = node->getFrustumable();
			// Skip if it is the same
			if(ANKI_UNLIKELY(frustumable == fr))
			{
				continue;
			}

			Spatial* sp = node->getSpatial();
			if(!sp)
			{
				continue;
			}

			if(!frustumable->insideFrustum(*sp))
			{
				continue;
			}

			Renderable* r = node->getRenderable();
			if(r)
			{
				if(!renderer->doVisibilityTests(
					sp->getOptimalCollisionShape()))
				{
					continue;
				}
				visible->renderables.push_back(node);
			}
			else
			{
				Light* l = node->getLight();
				if(l)
				{
					visible->lights.push_back(node);

					if(l->getShadowEnabled() && fr)
					{
						testLight(*l);
					}
				}
			}

			sp->enableFlags(Spatial::SF_VISIBLE_CAMERA);
		} // end for
	}

	/// Test an individual light
	void testLight(Light& light)
	{
		Frustumable& ref = *light.getFrustumable();
		ANKI_ASSERT(&ref != nullptr);

		// Allocate new visibles
		VisibilityTestResults* lvisible = 
			ANKI_NEW(VisibilityTestResults, frameAlloc, frameAlloc);

		ref.setVisibilityTestResults(lvisible);

		for(auto it = nodes; it != nodes + nodesCount; it++)
		{
			SceneNode* node = *it;

			Frustumable* fr = node->getFrustumable();
			// Wont check the same
			if(ANKI_UNLIKELY(&ref == fr))
			{
				continue;
			}

			Spatial* sp = node->getSpatial();
			if(!sp)
			{
				continue;
			}

			if(!ref.insideFrustum(*sp))
			{
				continue;
			}

			sp->enableFlags(Spatial::SF_VISIBLE_LIGHT);

			Renderable* r = node->getRenderable();
			if(r)
			{
				lvisible->renderables.push_back(node);
			}
		}
	}
};

//==============================================================================
void doVisibilityTests(SceneNode& fsn, Scene& scene, 
	Renderer& r)
{
	Frustumable* fr = fsn.getFrustumable();
	ANKI_ASSERT(fr);

	//
	// Do the tests in parallel
	//
	ThreadPool& threadPool = ThreadPoolSingleton::get();
	VisibilityTestJob jobs[ThreadPool::MAX_THREADS];
	for(U i = 0; i < threadPool.getThreadsCount(); i++)
	{
		jobs[i].nodesCount = scene.getSceneNodesCount();
		jobs[i].nodes = scene.getSceneNodesBegin();
		jobs[i].frustumable = fr;
		jobs[i].renderer = &r;
		jobs[i].frameAlloc = scene.getFrameAllocator();

		threadPool.assignNewJob(i, &jobs[i]);
	}

	threadPool.waitForAllJobsToFinish();

	//
	// Combine results
	//

	// Count the visible scene nodes to optimize the allocation of the 
	// final result
	U32 renderablesSize = 0;
	U32 lightsSize = 0;
	for(U i = 0; i < threadPool.getThreadsCount(); i++)
	{
		renderablesSize += jobs[i].visible->renderables.size();
		lightsSize += jobs[i].visible->lights.size();
	}

	// Allocate
	VisibilityTestResults* visible = 
		ANKI_NEW(VisibilityTestResults, scene.getFrameAllocator(), 
		scene.getFrameAllocator(), 
		renderablesSize, 
		lightsSize);

	// Append thread results
	for(U i = 0; i < threadPool.getThreadsCount(); i++)
	{
		visible->renderables.insert(
			visible->renderables.end(),
			jobs[i].visible->renderables.begin(), 
			jobs[i].visible->renderables.end());

		visible->lights.insert(
			visible->lights.end(),
			jobs[i].visible->lights.begin(), 
			jobs[i].visible->lights.end());
	}

	// Set the frustumable
	fr->setVisibilityTestResults(visible);

	//
	// Sort
	//

	// The lights
	DistanceSortJob dsjob;
	dsjob.nodes = visible->lights.begin();
	dsjob.nodesCount = visible->lights.size();
	dsjob.origin = fr->getFrustumableOrigin();
	threadPool.assignNewJob(0, &dsjob);

	// The rest of the jobs are dummy
	ThreadJobDummy dummyjobs[ThreadPool::MAX_THREADS];
	for(U i = 1; i < threadPool.getThreadsCount(); i++)
	{
		threadPool.assignNewJob(i, &dummyjobs[i]);
	}

	// Sort the renderables in the main thread
	std::sort(visible->renderables.begin(), 
		visible->renderables.end(), MaterialSortFunctor());

	threadPool.waitForAllJobsToFinish();
}

} // end namespace anki