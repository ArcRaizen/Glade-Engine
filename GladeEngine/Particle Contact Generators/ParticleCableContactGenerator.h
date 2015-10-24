#pragma once
#ifndef GLADE_PARTICLE_CABLE_CONTACT_GENERATOR
#define GLADE_PARTICLE_CABLE_CONTACT_GENERATOR

#ifndef GLADE_PARTICLE_LINK_CONTACT_GENERATOR
#include "ParticleLinkContactGenerator.h"
#endif

namespace Glade {
/*
	Cable that Links 2 Particles together.
	Generates a ParticleContact if they are too far apart, linking them together.
*/
class ParticleCableContactGenerator : public ParticleLinkContactGenerator
{
public:
	ParticleCableContactGenerator(Particle* p1_, Particle* p2_, gFloat len, gFloat res);
	Vector GetP1() { return p1->GetPosition(); }
	Vector GetP2() { return p2->GetPosition(); }
	virtual unsigned int GenerateContact(ParticleContact* contacts, unsigned int limit) const;

protected:
	gFloat	maxLength;
	gFloat	coeffRestitution;
};
}	// namespadce
#endif	// GLADE_PARTICLE_CABLE_CONTACT_GENERATOR

