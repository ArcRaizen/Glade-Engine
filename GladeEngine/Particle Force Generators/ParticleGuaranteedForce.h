#pragma once
#ifndef GLADE_PARTICLE_GUARANTEED_FORCE_H
#define GLADE_PARTICLE_GUARANTEED_FORCE_H

#ifndef GLADE_PARTICLE_FORCE_GENERATOR_H
#include "ParticleForceGenerator.h"
#endif

namespace Glade {
/*
	A Particle Force Generator that applies a specific force to a particle. 
*/
class ParticleGuaranteedForce : public ParticleForceGenerator
{
public:
	ParticleGuaranteedForce(const Vector& f);
	~ParticleGuaranteedForce();

	virtual void GeneratorForce(Particle* particle);

protected:
	const Vector force;
};
}	// namespace
#endif	// GLADE_PARTICLE_GUARANTEED_FORCE_H

