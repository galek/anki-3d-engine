#include "Light.h"
#include "collision.h"
#include "LightProps.h"
#include "App.h"
#include "MainRenderer.h"


//======================================================================================================================
// init [PointLight]                                                                                                  =
//======================================================================================================================
void PointLight::init(const char* filename)
{
	lightProps.loadRsrc(filename);
	radius = lightProps->getRadius();
}


//======================================================================================================================
// init [SpotLight]                                                                                                    =
//======================================================================================================================
void SpotLight::init(const char* filename)
{
	lightProps.loadRsrc(filename);
	camera.setAll(lightProps->getFovX(), lightProps->getFovY(), 0.2, lightProps->getDistance());
	castsShadow = lightProps->castsShadow();

	if(lightProps->getTexture() == NULL)
	{
		ERROR("Light properties \"" << lightProps->getRsrcName() << "\" do not have a texture");
		return;
	}
}


//======================================================================================================================
// render                                                                                                              =
//======================================================================================================================
void Light::render()
{
	Renderer::Dbg::drawSphere(0.1, getWorldTransform(), Vec4(lightProps->getDiffuseColor(), 1.0));
	//Renderer::Dbg::drawSphere(0.1, Transform::getIdentity(), Vec4(lightProps->getDiffuseColor(), 1.0));
}
