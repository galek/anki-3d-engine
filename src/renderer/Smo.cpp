#include "anki/renderer/Smo.h"
#include "anki/renderer/Renderer.h"
#include "anki/scene/Light.h"
#include "anki/scene/Scene.h"
#include "anki/resource/Mesh.h"

namespace anki {

//==============================================================================

static const float THRESHOLD = 0.2;

//==============================================================================
Smo::Geom::Geom()
{}

//==============================================================================
Smo::Geom::~Geom()
{}

//==============================================================================
Smo::~Smo()
{}

//==============================================================================
void Smo::init(const RendererInitializer& /*initializer*/)
{
	sProg.load("shaders/IsSmo.glsl");

	// Geometry stuff
	//

	// Sphere
	sphereGeom.mesh.load("engine-rsrc/sphere.mesh");
	sphereGeom.vao.create();
	sphereGeom.vao.attachArrayBufferVbo(
		*sphereGeom.mesh->getVbo(Mesh::VBO_POSITIONS),
		sProg->findAttributeVariable("position"), 3, GL_FLOAT,
		GL_FALSE, 0, NULL);
	sphereGeom.vao.attachElementArrayBufferVbo(
		*sphereGeom.mesh->getVbo(Mesh::VBO_INDICES));

	// Cameras
	std::array<const char*, Camera::CT_COUNT> files = {{
		"engine-rsrc/pyramid.mesh", "engine-rsrc/cube.mesh"}};

	for(int i = 0; i < Camera::CT_COUNT; i++)
	{
		camGeom[i].mesh.load(files[i]);
		camGeom[i].vao.create();
		camGeom[i].vao.attachArrayBufferVbo(
			*camGeom[i].mesh->getVbo(Mesh::VBO_POSITIONS),
			sProg->findAttributeVariable("position"), 3, GL_FLOAT,
			GL_FALSE, 0, NULL);
		camGeom[i].vao.attachElementArrayBufferVbo(
			*camGeom[i].mesh->getVbo(Mesh::VBO_INDICES));
	}
}

//==============================================================================
void Smo::setupGl(bool inside)
{
	glStencilFunc(GL_ALWAYS, 0x1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glClear(GL_STENCIL_BUFFER_BIT);

	if(inside)
	{
		glCullFace(GL_FRONT);
	}
	else
	{
		glDepthMask(GL_FALSE);
	}

	GlStateSingleton::get().enable(GL_DEPTH_TEST, !inside);
}

//==============================================================================
void Smo::restoreGl(bool inside)
{
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilFunc(GL_EQUAL, 0x1, 0x1);

	if(inside)
	{
		glCullFace(GL_BACK);
	}
	else
	{
		glDepthMask(GL_TRUE);
	}
}

//==============================================================================
void Smo::run(const PointLight& light)
{
	// set GL state
	const Camera& cam = r->getScene().getActiveCamera();
	const Vec3& o = light.getWorldTransform().getOrigin();
	const Vec3& c = cam.getWorldTransform().getOrigin();
	bool inside =  (o - c).getLength() <=
		(light.getRadius() + cam.getNear() + THRESHOLD);
	setupGl(inside);

	// set shared prog
	static const float SCALE = 1.0; // we scale the sphere a little
	sProg->bind();
	Mat4 modelMat = Mat4(light.getWorldTransform().getOrigin(),
	Mat3::getIdentity(), light.getRadius() * SCALE);
	Mat4 trf = cam.getViewProjectionMatrix() * modelMat;
	sProg->findUniformVariable("modelViewProjectionMat").set(trf);

	// render sphere to the stencil buffer
	sphereGeom.vao.bind();
	glDrawElements(GL_TRIANGLES, sphereGeom.mesh->getIndicesNumber(0),
		GL_UNSIGNED_SHORT, 0);
	sphereGeom.vao.unbind();

	// restore GL
	restoreGl(inside);

}

} // end namespace anki