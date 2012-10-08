#include "anki/renderer/Sm.h"
#include "anki/renderer/Renderer.h"
#include "anki/core/App.h"
#include "anki/scene/Scene.h"
#include "anki/scene/Camera.h"
#include "anki/scene/Light.h"

namespace anki {

//==============================================================================
void Sm::init(const RendererInitializer& initializer)
{
	enabled = initializer.is.sm.enabled;

	if(!enabled)
	{
		return;
	}

	pcfEnabled = initializer.is.sm.pcfEnabled;
	bilinearEnabled = initializer.is.sm.bilinearEnabled;
	resolution = initializer.is.sm.resolution;

	// Init the shadowmaps
	if(initializer.is.sm.maxLights > MAX_SHADOW_CASTERS)
	{
		throw ANKI_EXCEPTION("Too many shadow casters");
	}

	sms.resize(initializer.is.sm.maxLights);
	for(Shadowmap& sm : sms)
	{
		Renderer::createFai(resolution, resolution,
			GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, sm.tex);

		if(bilinearEnabled)
		{
			sm.tex.setFiltering(Texture::TFT_LINEAR);
		}
		else
		{
			sm.tex.setFiltering(Texture::TFT_NEAREST);
		}

		sm.tex.setParameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		sm.tex.setParameter(GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

		sm.fbo.create();
		sm.fbo.setOtherAttachment(GL_DEPTH_ATTACHMENT, sm.tex);
	}
}

//==============================================================================
void Sm::prepareDraw()
{
	// set GL
	GlStateSingleton::get().setViewport(0, 0, resolution, resolution);

	// disable color & blend & enable depth test
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	GlStateSingleton::get().enable(GL_DEPTH_TEST);
	GlStateSingleton::get().disable(GL_BLEND);

	// for artifacts
	glPolygonOffset(2.0, 2.0); // keep the values as low as possible!!!!
	GlStateSingleton::get().enable(GL_POLYGON_OFFSET_FILL);
}

//==============================================================================
void Sm::afterDraw()
{
	GlStateSingleton::get().disable(GL_POLYGON_OFFSET_FILL);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

//==============================================================================
void Sm::run(Light* shadowCasters[], U32 shadowCastersCount, 
	Texture* shadowmaps[])
{
	ANKI_ASSERT(enabled);

	prepareDraw();

	// render all
	for(U32 i = 0; i < shadowCastersCount; i++)
	{
		Texture* sm = doLight(*shadowCasters[i]);
		ANKI_ASSERT(sm != nullptr);
		shadowmaps[i] = sm;
	}

	afterDraw();
}

//==============================================================================
Sm::Shadowmap& Sm::bestCandidate(Light& light)
{
	// Allready there
	for(Shadowmap& sm : sms)
	{
		if(&light == sm.light)
		{
			return sm;
		}
	}

	// Find a null
	for(Shadowmap& sm : sms)
	{
		if(sm.light == nullptr)
		{
			sm.light = &light;
			sm.timestamp = 0;
			return sm;
		}
	}

	// Find an old and replace it
	Shadowmap* sm = &sms[0];
	for(U i = 1; i < sms.size(); i++)
	{
		if(sms[i].timestamp < sm->timestamp)
		{
			sm = &sms[i];
		}
	}

	sm->light = &light;
	sm->timestamp = 0;
	return *sm;
}

//==============================================================================
Texture* Sm::doLight(Light& light)
{
	Shadowmap& sm = bestCandidate(light);
	Texture* outSm = &sm.tex;

	Frustumable* fr = light.getFrustumable();
	ANKI_ASSERT(fr != nullptr);
	Movable* mov = &light;
	VisibilityInfo& vi = fr->getVisibilityInfo();

	//
	// Find last update
	//
	U32 lastUpdate = light.getMovableTimestamp();
	lastUpdate = std::max(lastUpdate, fr->getFrustumableTimestamp());

	for(auto it = vi.getRenderablesBegin(); it != vi.getRenderablesEnd(); ++it)
	{
		SceneNode* node = *it;
		Frustumable* bfr = node->getFrustumable();
		Movable* bmov = node->getMovable();

		if(bfr)
		{
			lastUpdate = std::max(lastUpdate, bfr->getFrustumableTimestamp());
		}

		if(bmov)
		{
			lastUpdate = std::max(lastUpdate, bmov->getMovableTimestamp());
		}
	}

	Bool shouldUpdate = lastUpdate >= sm.timestamp;

	if(!shouldUpdate)
	{
		return outSm;
	}

	sm.timestamp = Timestamp::getTimestamp();

	//
	// Render
	//
	sm.fbo.bind();

	for(auto it = vi.getRenderablesBegin(); it != vi.getRenderablesEnd(); ++it)
	{
		r->getSceneDrawer().render(*fr, 1, *((*it)->getRenderable()));
	}

	return outSm;
}

} // end namespace anki
