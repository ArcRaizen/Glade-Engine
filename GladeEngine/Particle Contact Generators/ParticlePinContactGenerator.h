#pragma once
#ifndef GLADE_PARTICLE_PIN_CONTACT_GENERATOR_H
#define GLADE_PARTICLE_PIN_CONTACT_GENERATOR_H

#ifndef GLADE_PARTICLE_CONTACT_GENERATOR_H
#include "ParticleContactGenerator.h"
#endif

namespace Glade {
/*

*/
class ParticlePinContactGenerator : public ParticleContactGenerator
{
public:
	ParticlePinContactGenerator(Particle* p1_, Vector p);

	unsigned int GenerateContact(ParticleContact* contacts, unsigned int limit) const;

protected:
	Particle* p1;
	Vector pin;
};
}	// namespace
#endif	// GLADE_PARTICLE_PIN_CONTACT_GENERATOR_H

