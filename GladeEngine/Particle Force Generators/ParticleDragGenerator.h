#pragma once
#ifndef GLADE_PARTICLE_DRAG_GENERATOR_H
#define GLADE_PARTICLE_DRAG_GENERATOR_H

#ifndef GLADE_PARTICLE_FORCE_GENERATOR_H
#include "ParticleForceGenerator.h"
#endif

namespace Glade {
/*
	A Particle Force Generator that exerts a drag force on a particle
	based on that particle's velocity.
*/
class ParticleDragGenerator : public ParticleForceGenerator
{
public:
	ParticleDragGenerator(const gFloat k1_, const gFloat k2_);
	~ParticleDragGenerator();

	virtual void GenerateForce(Particle* particle);

private:
	const gFloat k1;
	const gFloat k2;
};
}	// namespace
#endif	// GLADE_PARTICLE_DRAG_GENERATOR

