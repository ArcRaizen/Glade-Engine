#pragma once
#ifndef GLADE_PARTICLE_CONTACT_GENERATOR_H
#define GLADE_PARTICLE_CONTACT_GENERATOR_H

#ifndef GLADE_PARTICLE_CONTACT_H
#include "ParticleContact.h"
#endif

namespace Glade {
/*
	Base Interface for Contact Generators on Particles
*/
class ParticleContactGenerator
{
public:
	/* Generates Contact and adds it to ParticleContact array passed into function
	   if any Particle(s) violate the constraints of any ParticleContactGenerators that 
	   inherit from this interface.
	
	   Contacts is a pointer to an array of ParticleContacts that this function populates
	   Limit is the maximum size of the ParticleContacts array
	
	   Returns the number of contacts generated
	   Return -1 to remove the ParticleContactGenerator permanently
	*/
	virtual unsigned int GenerateContact(ParticleContact* contacts, unsigned int limit) const = 0;
};
}	// namespace
#endif	// GLADE_PARTICLE_CONTACT_GENERATOR_H
