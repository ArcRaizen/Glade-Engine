#pragma once
#ifndef GLADE_PARTICLE_ROD_CONTACT_GENERATOR_H
#define GLADE_PARTICLE_ROD_CONTACT_GENERATOR_H

#ifndef GLADE_PARTICLE_LINK_CONTACT_GENERATOR_H
#include "ParticleLinkContactGenerator.h"
#endif

namespace Glade {
/*
	Rod that Links 2 Particles together.
	Generates a ParticleContact if they are too far apart or too close together, linking them together.
*/
class ParticleRodContactGenerator : public ParticleLinkContactGenerator
{
public:
	ParticleRodContactGenerator(Particle* p1_, Particle* p2_, gFloat len);

	virtual unsigned int GenerateContact(ParticleContact* contacts, unsigned int limit) const;

protected:
	gFloat	length;		// length of the rod holding the 2 Particles together
};
}	// namespace
#endif	// GLADE_PARTICLE_ROD_CONTACT_GENERATOR_H

