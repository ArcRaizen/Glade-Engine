#pragma once
#ifndef GLADE_PARTICLE_FORCE_GENERATOR_H
#define GLADE_PARTICLE_FORCE_GENERATOR_H

#ifndef GLADE_PARTICLE_H
#include "..\Particle.h"
#endif
#include <functional>

namespace Glade {
class ParticleForceGenerator
{
public:
	ParticleForceGenerator();
	~ParticleForceGenerator();

	// Exert a force over 'dt' time on particle p
	virtual void GenerateForce(Particle* p) = 0;

	int GetID() { return forceGeneratorID; }
protected:
	static int ForceGeneratorID;
	int forceGeneratorID;
};
}	// namespace
#endif	// GLADE_PARTICLE_FORCE_GENERATOR_H
