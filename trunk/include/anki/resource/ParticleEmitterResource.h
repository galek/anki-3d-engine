#ifndef ANKI_RESOURCE_PARTICLE_EMITTER_RSRC_H
#define ANKI_RESOURCE_PARTICLE_EMITTER_RSRC_H

#include "anki/Math.h"
#include "anki/resource/Resource.h"

namespace anki {

class XmlElement;

/// The particle emitter properties. Different class from
/// ParticleEmitterResource so it can be inherited
class ParticleEmitterProperties
{
public:
	ParticleEmitterProperties()
	{}

	ParticleEmitterProperties(const ParticleEmitterProperties& b)
	{
		*this = b;
	}

	ParticleEmitterProperties& operator=(const ParticleEmitterProperties& b);

	/// @name Particle specific properties
	/// @{
	class
	{
	public:
		/// Particle life
		F32 m_life = 10.0;
		F32 m_lifeDeviation = 0.0;

		/// Particle mass
		F32 m_mass = 1.0;
		F32 m_massDeviation = 0.0;

		/// Particle size. It is the size of the collision shape
		F32 m_size = 1.0;
		F32 m_sizeDeviation = 0.0;
		F32 m_sizeAnimation = 1.0;

		/// Alpha factor. If the material supports alpha then multiply with 
		/// this
		F32 m_alpha = 1.0;
		F32 m_alphaDeviation = 0.0;
		Bool8 m_alphaAnimation = false;

		/// Initial force. If not set only the gravity applies
		Vec3 m_forceDirection = Vec3(0.0, 1.0, 0.0);
		Vec3 m_forceDirectionDeviation = Vec3(0.0);
		F32 m_forceMagnitude = 0.0; ///< Default 0.0
		F32 m_forceMagnitudeDeviation = 0.0;

		/// If not set then it uses the world's default
		Vec3 m_gravity = Vec3(0.0);
		Vec3 m_gravityDeviation = Vec3(0.0);

		/// This position is relevant to the particle emitter pos
		Vec3 m_startingPos = Vec3(0.0);
		Vec3 m_startingPosDeviation = Vec3(0.0);
	} m_particle;
	/// @}

	/// @name Emitter specific properties
	/// @{

	/// The size of the particles vector. Required
	U32 m_maxNumOfParticles = 16;
	/// How often the emitter emits new particles. In secs. Required
	F32 m_emissionPeriod = 1.0;
	/// How many particles are emitted every emission. Required
	U32 m_particlesPerEmittion = 1;
	/// Use bullet for the simulation
	Bool m_usePhysicsEngine = true;
	/// @}

	// Optimization flags
	Bool m_forceEnabled;
	Bool m_wordGravityEnabled;

	void updateFlags();
};

/// This is the properties of the particle emitter resource
class ParticleEmitterResource: private ParticleEmitterProperties
{
public:
	ParticleEmitterResource();
	~ParticleEmitterResource();

	const ParticleEmitterProperties& getProperties() const
	{
		return *this;
	}

	const Material& getMaterial() const
	{
		return *m_material;
	}
	Material& getMaterial()
	{
		return *m_material;
	}

	/// Load it
	void load(const char* filename);

private:
	MaterialResourcePointer m_material;

	void loadInternal(const XmlElement& el);
};

} // end namespace anki

#endif
