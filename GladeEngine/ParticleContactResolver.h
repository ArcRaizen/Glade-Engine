#pragma once
#ifndef GLADE_PARTICLE_CONTACT_RESOLVER_H
#define GLADE_PARTICLE_CONTACT_RESOLVER_H

#ifndef GLADE_PARTICLE_CONTACT_H
#include "ParticleContact.h"
#endif

namespace Glade {
/*
	Contact Resolution routine for ParticleContacts.
*/
class ParticleContactResolver
{
public:
	ParticleContactResolver(unsigned int iter);
	~ParticleContactResolver();

	void SetIterations(unsigned int iter);

	void ResolveContacts(ParticleContact* contactArray, unsigned int numContants);
	void SortContacts(ParticleContact* data, int start, int end);

protected:
	unsigned int iterations;
	unsigned int iterationsUsed;
};
}	// namespace
#endif	// GLADE_PARTICLE_CONTACT_RESOLVER_H

