#pragma once
#ifndef GLADE_PARTICLE_ATTRACTOR_FORCE_H
#define GLADE_PARTICLE_ATTRACTOR_FORCE_H

#ifndef GLADE_PARTICLE_GRAVITY_H
#include "ParticleGravity.h"
#endif

namespace Glade { 
class ParticleAttractorForce : public ParticleGravity
{
public:
	ParticleAttractorForce(const Vector grav, const Vector attract);
	~ParticleAttractorForce();

	void GenerateForce(Particle* particle);

protected:
	Vector attractionPoint;
};
}	// namespace
#endif	// GLADE_PARTICLE_ATTRACTOR_FORCE_H

