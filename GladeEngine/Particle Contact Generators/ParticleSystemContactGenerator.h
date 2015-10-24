#pragma once
#ifndef GLADE_PARTICLE_SYSTEM_CONTACT_GENERATOR_H
#define GLADE_PARTICLE_SYSTEM_CONTACT_GENERATOR_H

#ifndef GLADE_PARTICLE_CONTACT_GENERATOR_H
#include "ParticleContactGenerator.h"
#endif

namespace Glade {
/*
	Base Interface for Contact Generators on a Particle System
*/
class ParticleSystemContactGenerator : public ParticleContactGenerator
{
public:
	ParticleSystemContactGenerator(std::vector<Particle*>& system) : particleSystem(system) { }

	// Generates Contact and adds it to ParticleContact array passed into function
	// if any Particle(s) violate the constraints of any ParticleContactGenerators that 
	// inherit from this interface.
	virtual unsigned int GenerateContact(ParticleContact* contacts, unsigned int limit) const = 0;

protected:
	std::vector<Particle*> particleSystem;
};
}	// namespace
#endif	// GLADE_PARTICLE_SYSTEM_CONTACT_GENERATOR_H