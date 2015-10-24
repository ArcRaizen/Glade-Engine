#pragma once
#ifndef GLADE_PARTICLE_DIRECTIONAL_FORCE_GENERATOR_H
#define GLADE_PARTICLE_DIRECTIONAL_FORCE_GENERATOR_H

#ifndef GLADE_PARTICLE_FORCE_GENERATOR_H
#include "ParticleForceGenerator.h"
#endif

namespace Glade {
class ParticleDirectionalForceGenerator
{
public:
	enum class ForceScaling { NONE, LINEAR, SQUARED, LOG };

	ParticleDirectionalForceGenerator(const Vector o, const Vector dir, const Vector force, ForceScaling s, gFloat rad=G_MAX, bool push=true, gFloat maxDist=G_MAX);
	~ParticleDirectionalForceGenerator();

	virtual void GenerateForce(Particle* particle);

protected:
	Vector			origin;				// base position of the center of the force being exerted
	Vector			direction;			// direction from origin that the force is being exerted
	bool			pushFromOrigin;		// Does this force push from the origin or pull towards it
	Vector			forceAtOrigin;		// the maximum force being exerted
	gFloat			radius;				// max distance from origin perpendicular to 'distance' particles may be to be acted on by this
	gFloat			maxDistance;		// max distance from origin parallel to distance particles may be to be acted on by this
	ForceScaling	scaling;
};
}	// namespace
#endif	// GLADE_PARTICLE_DIRECTIONAL_FORCE_GENERATOR_H
