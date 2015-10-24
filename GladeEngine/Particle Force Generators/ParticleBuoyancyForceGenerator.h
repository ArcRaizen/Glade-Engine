#pragma once
#ifndef GLADE_PARTICLE_BUOYANCY_FORCE_GENERATOR_H
#define GLADE_PARTICLE_BUYOANCY_FORCE_GENERATOR_H

#ifndef GLADE_PARTICLE_DIRECTIONAL_FORCE_GENERATOR_H
#include "ParticleDirectionalForceGenerator.h"
#endif

namespace Glade {
class ParticleBuyoancyForceGenerator : public ParticleDirectionalForceGenerator
{
	ParticleBuyoancyForceGenerator(const Vector pointOnSurface, const Vector dir, gFloat depth, gFloat vol, gFloat density, gFloat rad=G_MAX, gFloat maxDist=G_MAX);
	~ParticleBuyoancyForceGenerator();

	void GenerateForce(Particle* particle);

protected:
	gFloat	maxDepth;
	gFloat	volume;
	gFloat	liquidDensity;
};
}	// namespace
#endif	// GLADE_PARTICLE_BUYOANCY_FORCE_GENERATOR_H