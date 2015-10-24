#pragma once
#ifndef GLADE_PARTICLE_TEARABLE_CABLE_CONTACT_GENERATOR_H
#define GLADE_PARTICLE_TEARABLE_CABLE_CONTACT_GENERATOR_H

#ifndef GLADE_PARTICLE_CABLE_CONTACT_GENERATOR
#include "ParticleCableContactGenerator.h"
#endif

namespace Glade { 
/*
	Cable that Links 2 Particles together.
	Generates a ParticleContact if they are too far apart, linking them together.

	If the Particles become too far apart, this ContactGenerator is destroyed, effectively
	tearing the cable.
*/
class ParticleTearableCableContactGenerator : public ParticleCableContactGenerator
{
public:
	ParticleTearableCableContactGenerator(Particle* p1_, Particle* p2_, gFloat len, gFloat res, gFloat tearLength);

	Vector GetP1() { return p1->GetPosition(); }
	Vector GetP2() { return p2->GetPosition(); }

	unsigned int GenerateContact(ParticleContact* contacts, unsigned int limit) const;
	
protected:
	gFloat tearLength;
};
}	// namespace
#endif	// GLADE_PARTICLE_TEARABLE_CABLE_CONTACT_GENERATOR_H

