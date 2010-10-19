#include "Controller.h"
#include "Scene.h"
#include "App.h"


//======================================================================================================================
// Constructor                                                                                                         =
//======================================================================================================================
Controller::Controller(ControllerType type_):
	type(type_) 
{
	app->getScene().registerController(this);
}


//======================================================================================================================
// Destructor                                                                                                          =
//======================================================================================================================
Controller::~Controller()
{
	app->getScene().unregisterController(this);
}
