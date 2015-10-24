#pragma once
#ifndef GLADE_PARTICLE_LINK_CONTACT_GENERATOR
#define GLADE_PARTICLE_LINK_CONTACT_GENERATOR

#ifndef GLADE_PARTICLE_CONTACT_GENERATOR
#include "ParticleContactGenerator.h"
#endif

namespace Glade {
/*
	Base Interface for ParticleContactGenerator that Links two particles together.
	Generates a ParticleContact if they violate the constraints of the link.
*/
class ParticleLinkContactGenerator : public ParticleContactGenerator
{
public:
	ParticleLinkContactGenerator(Particle* p1_, Particle* p2_) : p1(p1_), p2(p2_) { }

	// Generates Contact to keep link from being violated
	virtual unsigned int GenerateContact(ParticleContact* contacts, unsigned int limit) const = 0;

protected:
	// Returns current length of the link
	gFloat CalcCurrentLength() const { return (p1->GetPosition() - p2->GetPosition()).Magnitude(); }
	gFloat CalcCurrentSquaredLength() const { return (p1->GetPosition() - p2->GetPosition()).SquaredMagnitude(); }

	Particle* p1;		// First Particle linked to the Second Particle
	Particle* p2;		// Second Particle linked to first Particle
};
}	// namespace
#endif	// GLADE_PARTICLE_LINK_CONTACT_GENERATOR
