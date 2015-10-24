#pragma once
#ifndef GLADE_PARTICLE_SPRING_FORCE_GENERATOR_H
#define GLADE_PARTICLE_SPRING_FORCE_GENERATOR_H

#ifndef GLADE_PARTICLE_FORCE_GENERATOR_H
#include "ParticleForceGenerator.h"
#endif

namespace Glade {
/*
	A Particle Force Generator that exerts a force on a Particle
	like a spring based on the particles distance from other end of the spring
	and the spring's resting length and spring constant.

	By default, the spring force will exert when the spring is compressed and extended,
	but both can optionally be disabled to simulate more unique springs.
*/
class ParticleSpringForceGenerator : public ParticleForceGenerator
{
public:
	ParticleSpringForceGenerator(const Vector& v, gFloat k, gFloat l, bool extended=true, bool compressed=true);
	~ParticleSpringForceGenerator();

	virtual void GenerateForce(Particle* particle);

	void SetAnchor(const Vector& v);

protected:
	Vector	anchor;					// Opposite endpoint of spring from particle passed to GenerateForce
	gFloat	springConstant;			// Spring Constant of spring - determines strength of force
	gFloat	restLength;				// Length at which no force is exerted by the spring
	bool	exertWhileExtended;		// Does this spring exert force when extended?
	bool	exertWhileCompressed;	// Does this spring exert force when compressed?
};
}	// namespace
#endif	// GLADE_PARTICLE_SPRING_FORCE_GENERATOR_H

