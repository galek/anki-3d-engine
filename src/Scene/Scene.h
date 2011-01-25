#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include "Physics.h"
#include "Exception.h"
#include "Properties.h"
#include "Singleton.h"


class SceneNode;
class Light;
class Camera;
class Controller;
class ParticleEmitter;
class ModelNode;


/// The Scene contains all the dynamic entities
class Scene
{
	public:
		/// Typetraits
		template<typename Type>
		class Types
		{
			public:
				typedef Vec<Type*> Container;
				typedef typename Container::iterator Iterator;
				typedef typename Container::const_iterator ConstIterator;
		};

		// Containers of scene's data
		Types<SceneNode>::Container nodes;
		Types<Light>::Container lights;
		Types<Camera>::Container cameras;
		Types<ParticleEmitter>::Container particleEmitters;
		Types<ModelNode>::Container modelNodes;
		Types<Controller>::Container controllers;

		// The funcs
		Scene();
		~Scene() throw() {}

		void registerNode(SceneNode* node); ///< Put a node in the appropriate containers
		void unregisterNode(SceneNode* node);
		void registerController(Controller* controller);
		void unregisterController(Controller* controller);

		void updateAllWorldStuff();
		void updateAllControllers();

		/// @name Accessors
		/// @{
		GETTER_SETTER(Vec3, ambientCol, getAmbientCol, setAmbientCol)
		Physics& getPhysics() {return *physics;}
		const Physics& getPhysics() const {return *physics;}
		/// @}

	private:
		Vec3 ambientCol; ///< The global ambient color
		std::auto_ptr<Physics> physics; ///< Connection with Bullet wrapper

		/// Adds a node in a container
		template<typename ContainerType, typename Type>
		void putBackNode(ContainerType& container, Type* x);

		/// Removes a node from a container
		template<typename ContainerType, typename Type>
		void eraseNode(ContainerType& container, Type* x);
};


template<typename ContainerType, typename Type>
inline void Scene::putBackNode(ContainerType& container, Type* x)
{
	RASSERT_THROW_EXCEPTION(std::find(container.begin(), container.end(), x) != container.end());
	container.push_back(x);
}


template<typename ContainerType, typename Type>
inline void Scene::eraseNode(ContainerType& container, Type* x)
{
	typename ContainerType::iterator it = std::find(container.begin(), container.end(), x);
	RASSERT_THROW_EXCEPTION(it == container.end());
	container.erase(it);
}


//======================================================================================================================
// Singleton                                                                                                           =
//======================================================================================================================
typedef Singleton<Scene> SceneSingleton;


#endif
