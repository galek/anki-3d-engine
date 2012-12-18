#ifndef ANKI_RENDERER_DBG_H
#define ANKI_RENDERER_DBG_H

#include "anki/renderer/RenderingPass.h"
#include "anki/gl/Fbo.h"
#include <memory>
#include "anki/renderer/DebugDrawer.h"

namespace anki {

/// Debugging stage
class Dbg: public SwitchableRenderingPass
{
public:
	Dbg(Renderer* r_)
		: SwitchableRenderingPass(r_)
	{}
	
	~Dbg();

	void init(const RendererInitializer& initializer);
	void run();

	Bool getDepthTestEnabled() const
	{
		return depthTest;
	}
	void setDepthTestEnabled(Bool enable)
	{
		depthTest = enable;
	}

	DebugDrawer& getDebugDrawer()
	{
		return *drawer;
	}
	const DebugDrawer& getDebugDrawer() const
	{
		return *drawer;
	}

private:
	Fbo fbo;
	std::unique_ptr<DebugDrawer> drawer;
	// Have it as ptr because the constructor calls opengl
	std::unique_ptr<SceneDebugDrawer> sceneDrawer;
	Bool depthTest = true;
};

} // end namespace

#endif