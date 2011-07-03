#include "Particle.h"
#include "Physics/PhysRigidBody.h"


//==============================================================================
// Constructor                                                                 =
//==============================================================================
Particle::Particle(float timeOfDeath_, SceneNode* parent):
	ModelNode(parent),
	timeOfDeath(timeOfDeath_)
{}


//==============================================================================
// Destructor                                                                  =
//==============================================================================
Particle::~Particle()
{}


//==============================================================================
// setNewRigidBody                                                             =
//==============================================================================
void Particle::setNewRigidBody(Phys::RigidBody* body_)
{
	body.reset(body_);
}
