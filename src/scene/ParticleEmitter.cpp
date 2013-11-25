#include "anki/scene/ParticleEmitter.h"
#include "anki/scene/SceneGraph.h"
#include "anki/resource/Model.h"
#include "anki/util/Functions.h"
#include "anki/physics/PhysicsWorld.h"
#include "anki/gl/Drawcall.h"
#include <limits>

namespace anki {

//==============================================================================
// Misc                                                                        =
//==============================================================================

//==============================================================================
F32 getRandom(F32 initial, F32 deviation)
{
	return (deviation == 0.0) ? initial
		: initial + randFloat(deviation) * 2.0 - deviation;
}

//==============================================================================
Vec3 getRandom(const Vec3& initial, const Vec3& deviation)
{
	if(deviation == Vec3(0.0))
	{
		return initial;
	}
	else
	{
		Vec3 out;
		for(U i = 0; i < 3; i++)
		{
			out[i] = getRandom(initial[i], deviation[i]);
		}
		return out;
	}
}

//==============================================================================
// ParticleBase                                                                =
//==============================================================================

//==============================================================================
ParticleBase::ParticleBase(ParticleType type_)
	: type(type_)
{}

//==============================================================================
ParticleBase::~ParticleBase()
{}

//==============================================================================
void ParticleBase::revive(const ParticleEmitter& pe,
	F32 /*prevUpdateTime*/, F32 crntTime)
{
	ANKI_ASSERT(isDead());
	const ParticleEmitterProperties& props = pe;

	// life
	timeOfDeath = getRandom(crntTime + props.particle.life,
		props.particle.lifeDeviation);
	timeOfBirth = crntTime;
}

//==============================================================================
// ParticleSimple                                                              =
//==============================================================================

//==============================================================================
ParticleSimple::ParticleSimple()
	: ParticleBase(PT_SIMPLE)
{}

//==============================================================================
void ParticleSimple::simulate(const ParticleEmitter& pe,
	F32 prevUpdateTime, F32 crntTime)
{
	F32 dt = crntTime - prevUpdateTime;

	ANKI_ASSERT(
		static_cast<const ParticleEmitterProperties&>(pe).
		particle.gravity.getLength() > 0.0);

	Vec3 xp = position;
	Vec3 xc = acceleration * (dt * dt) + velocity * dt + xp;

	position = xc;

	velocity += acceleration * dt;
}

//==============================================================================
void ParticleSimple::revive(const ParticleEmitter& pe,
	F32 prevUpdateTime, F32 crntTime)
{
	ParticleBase::revive(pe, prevUpdateTime, crntTime);
	velocity = Vec3(0.0);

	const ParticleEmitterProperties& props = pe;

	acceleration = getRandom(props.particle.gravity,
			props.particle.gravityDeviation);

	// Set the initial position
	position = getRandom(props.particle.startingPos,
		props.particle.startingPosDeviation);

	position += pe.getWorldTransform().getOrigin();
}

//==============================================================================
// Particle                                                                    =
//==============================================================================

#if 0

//==============================================================================
Particle::Particle(
	const char* name, SceneGraph* scene, // SceneNode
	// RigidBody
	PhysicsWorld* masterContainer, const RigidBody::Initializer& init_)
	:	ParticleBase(name, scene, PT_PHYSICS)
{
	RigidBody::Initializer init = init_;

	getSceneGraph().getPhysics().newPhysicsObject<RigidBody>(body, init);

	sceneNodeProtected.rigidBodyC = body;
}

//==============================================================================
Particle::~Particle()
{
	getSceneGraph().getPhysics().deletePhysicsObject(body);
}

//==============================================================================
void Particle::revive(const ParticleEmitter& pe,
	F32 prevUpdateTime, F32 crntTime)
{
	ParticleBase::revive(pe, prevUpdateTime, crntTime);

	const ParticleEmitterProperties& props = pe;

	// pre calculate
	Bool forceFlag = props.forceEnabled;
	Bool worldGravFlag = props.wordGravityEnabled;

	// activate it (Bullet stuff)
	body->forceActivationState(ACTIVE_TAG);
	body->activate();
	body->clearForces();
	body->setLinearVelocity(btVector3(0.0, 0.0, 0.0));
	body->setAngularVelocity(btVector3(0.0, 0.0, 0.0));

	// force
	if(forceFlag)
	{
		Vec3 forceDir = getRandom(props.particle.forceDirection,
			props.particle.forceDirectionDeviation);
		forceDir.normalize();

		if(!pe.identityRotation)
		{
			// the forceDir depends on the particle emitter rotation
			forceDir = pe.getWorldTransform().getRotation() * forceDir;
		}

		F32 forceMag = getRandom(props.particle.forceMagnitude,
			props.particle.forceMagnitudeDeviation);

		body->applyCentralForce(toBt(forceDir * forceMag));
	}

	// gravity
	if(!worldGravFlag)
	{
		body->setGravity(toBt(getRandom(props.particle.gravity,
			props.particle.gravityDeviation)));
	}

	// Starting pos. In local space
	Vec3 pos = getRandom(props.particle.startingPos,
		props.particle.startingPosDeviation);

	if(pe.identityRotation)
	{
		pos += pe.getWorldTransform().getOrigin();
	}
	else
	{
		pos.transform(pe.getWorldTransform());
	}

	btTransform trf(
		toBt(Transform(pos, pe.getWorldTransform().getRotation(), 1.0)));
	body->setWorldTransform(trf);
}
#endif

//==============================================================================
// ParticleEmitter                                                             =
//==============================================================================

//==============================================================================
ParticleEmitter::ParticleEmitter(
	const char* name, SceneGraph* scene,
	const char* filename)
	:	SceneNode(name, scene),
		SpatialComponent(this),
		MoveComponent(this),
		RenderComponent(this),
		particles(getSceneAllocator())
{
	addComponent(static_cast<MoveComponent*>(this));
	addComponent(static_cast<SpatialComponent*>(this));
	addComponent(static_cast<RenderComponent*>(this));

	// Load resource
	particleEmitterResource.load(filename);

	// copy the resource to me
	ParticleEmitterProperties& me = *this;
	const ParticleEmitterProperties& other =
		particleEmitterResource->getProperties();
	me = other;

	if(usePhysicsEngine)
	{
		createParticlesSimulation(scene);
	}
	else
	{
		createParticlesSimpleSimulation(scene);
	}

	timeLeftForNextEmission = 0.0;
	RenderComponent::init();

	// Create the vertex buffer and object
	//
	const U components = 3 + 1 + 1;
	PtrSize vertSize = components * sizeof(F32);

	vbo.create(GL_ARRAY_BUFFER, maxNumOfParticles * vertSize,
		nullptr, GL_DYNAMIC_DRAW);

	vao.create();
	// Position
	vao.attachArrayBufferVbo(&vbo, 0, 3, GL_FLOAT, GL_FALSE, vertSize, 0);

	// Scale
	vao.attachArrayBufferVbo(&vbo, 6, 1, GL_FLOAT, GL_FALSE, vertSize, 
		sizeof(F32) * 3);

	// Alpha
	vao.attachArrayBufferVbo(&vbo, 7, 1, GL_FLOAT, GL_FALSE, vertSize, 
		sizeof(F32) * 4);
}

//==============================================================================
ParticleEmitter::~ParticleEmitter()
{
	// XXX delete particles
	/*for(ParticleBase* part : particles)
	{
		getSceneGraph().deleteSceneNode(part);
	}

	getSceneAllocator().deleteInstance(collShape);*/
}

//==============================================================================
void ParticleEmitter::getRenderingData(
	const PassLodKey& key, 
	const U8* subMeshIndicesArray, U subMeshIndicesCount,
	const Vao*& vao_, const ShaderProgram*& prog,
	Drawcall& dc)
{
	vao_ = &vao;
	prog = &getMaterial().findShaderProgram(key);

	dc.primitiveType = GL_POINTS;
	dc.indicesType = 0;

	dc.instancesCount = 1;
	dc.drawCount = 1;

	dc.count = aliveParticlesCount;
	dc.offset = 0;
}

//==============================================================================
const Material& ParticleEmitter::getMaterial()
{
	return particleEmitterResource->getMaterial();
}

//==============================================================================
void ParticleEmitter::componentUpdated(SceneComponent& comp, 
	SceneComponent::UpdateType)
{
	if(comp.getType() == MoveComponent::getGlobType())
	{
		identityRotation =
			getWorldTransform().getRotation() == Mat3::getIdentity();
	}
}

//==============================================================================
void ParticleEmitter::createParticlesSimulation(SceneGraph* scene)
{
#if 0
	collShape = getSceneAllocator().newInstance<btSphereShape>(particle.size);

	RigidBody::Initializer binit;
	binit.shape = collShape;
	binit.group = PhysicsWorld::CG_PARTICLE;
	binit.mask = PhysicsWorld::CG_MAP;

	particles.reserve(maxNumOfParticles);

	for(U i = 0; i < maxNumOfParticles; i++)
	{
		binit.mass = getRandom(particle.mass, particle.massDeviation);

		Particle* part;
		getSceneGraph().newSceneNode(part,
			nullptr, &scene->getPhysics(), binit);

		part->size = getRandom(particle.size, particle.sizeDeviation);
		part->alpha = getRandom(particle.alpha, particle.alphaDeviation);
		part->getRigidBody()->forceActivationState(DISABLE_SIMULATION);

		particles.push_back(part);
	}
#endif
}

//==============================================================================
void ParticleEmitter::createParticlesSimpleSimulation(SceneGraph* scene)
{
	for(U i = 0; i < maxNumOfParticles; i++)
	{
		ParticleSimple* part = 
			getSceneAllocator().newInstance<ParticleSimple>();

		part->size = getRandom(particle.size, particle.sizeDeviation);
		part->alpha = getRandom(particle.alpha, particle.alphaDeviation);

		particles.push_back(part);
	}
}

//==============================================================================
void ParticleEmitter::frameUpdate(F32 prevUpdateTime, F32 crntTime, 
	SceneNode::UpdateType uptype)
{
	if(uptype != SceneNode::ASYNC_UPDATE)
	{
		return;
	}

	// - Deactivate the dead particles
	// - Calc the AABB
	// - Calc the instancing stuff
	//
	Vec3 aabbmin(std::numeric_limits<F32>::max());
	Vec3 aabbmax(-std::numeric_limits<F32>::max());
	aliveParticlesCount = 0;

	// Create the alpha vectors
	SceneFrameVector<F32> alpha(getSceneFrameAllocator());
	alpha.reserve(particles.size());

	F32* verts = (F32*)vbo.map(GL_MAP_WRITE_BIT);

	for(ParticleBase* p : particles)
	{
		if(p->isDead())
		{
			// if its already dead so dont deactivate it again
			continue;
		}

		if(p->getTimeOfDeath() < crntTime)
		{
			// Just died
			p->kill();
		}
		else
		{
			// This will calculate a new world transformation
			p->simulate(*this, prevUpdateTime, crntTime);

			// An alive
			const Vec3& origin = p->getPosition();

			for(U i = 0; i < 3; i++)
			{
				aabbmin[i] = std::min(aabbmin[i], origin[i]);
				aabbmax[i] = std::max(aabbmax[i], origin[i]);
			}

			F32 lifePercent = (crntTime - p->getTimeOfBirth())
				/ (p->getTimeOfDeath() - p->getTimeOfBirth());

			verts[0] = origin.x();
			verts[1] = origin.y();
			verts[2] = origin.z();

			// XXX set a flag for scale
			verts[3] = p->size + (lifePercent * particle.sizeAnimation);

			// Set alpha
			verts[4] = sin((lifePercent) * getPi<F32>()) * p->alpha;

			++aliveParticlesCount;
			verts += 4;
		}
	}

	vbo.unmap();

	if(aliveParticlesCount != 0)
	{
		aabb = Aabb(aabbmin - particle.size, aabbmax + particle.size);
	}
	else
	{
		aabb = Aabb(Vec3(0.0), Vec3(0.01));
	}
	SpatialComponent::markForUpdate();

	//
	// Emit new particles
	//
	if(timeLeftForNextEmission <= 0.0)
	{
		U particlesCount = 0; // How many particles I am allowed to emmit
		for(ParticleBase* pp : particles)
		{
			ParticleBase& p = *pp;
			if(!p.isDead())
			{
				// its alive so skip it
				continue;
			}

			p.revive(*this, prevUpdateTime, crntTime);

			// do the rest
			++particlesCount;
			if(particlesCount >= particlesPerEmittion)
			{
				break;
			}
		} // end for all particles

		timeLeftForNextEmission = emissionPeriod;
	} // end if can emit
	else
	{
		timeLeftForNextEmission -= crntTime - prevUpdateTime;
	}
}

} // end namespace anki
