#include "ScriptingCommon.h"
#include "scene/Scene.h"
#include "scene/Camera.h"
#include "scene/ModelNode.h"


WRAP(Scene)
{
	WRAP_CONTAINER(Scene::Types<Camera>::Container)
	WRAP_CONTAINER(Scene::Types<ModelNode>::Container)

	class_<Scene, noncopyable>("Scene", no_init)
		.def("setAmbientColor", &Scene::setAmbientColor)
		.def("getAmbientColor", (const Vec3& (Scene::*)() const)(&
			Scene::getAmbientColor),
			return_value_policy<reference_existing_object>())

		.def("getCameras", (Scene::Types<Camera>::Container& (Scene::*)())(&
			Scene::getCameras),
			return_value_policy<reference_existing_object>())

		.def("getModelNodes", (Scene::Types<ModelNode>::Container&
			(Scene::*)())(&Scene::getModelNodes),
			return_value_policy<reference_existing_object>())
	;
}