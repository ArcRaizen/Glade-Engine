#pragma once
#ifndef GLADE_PARTICLE_GRAVITY_H
#define GLADE_PARTICLE_GRAVITY_H

#ifndef GLADE_PARTICLE_GUARANTEED_FORCE_H
#include "ParticleGuaranteedForce.h"
#endif

namespace Glade {
/*
	A Particle Force Generator that exerts gravity on a particle.
*/
class ParticleGravity : public ParticleGuaranteedForce
{
public:
	ParticleGravity(const Vector grav=Vector::GRAVITY);
	~ParticleGravity();

	void GenerateForce(Particle* particle);
};
}	// namespace
#endif	// GLADE_PARTICLE_GRAVITY_H

