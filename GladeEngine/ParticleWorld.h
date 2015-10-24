#pragma once
#ifndef GLADE_PARTICLE_WORLD_H
#define GLADE_PARTICLE_WORLD_H

#include "Particle Force Generators\ParticleForceGenerator.h"
#include "Particle Contact Generators\ParticleContactGenerator.h"
#include "ParticleContactResolver.h"
#include <map>

namespace Glade { 
/*
	Keeps track of a set of Particles and provides the means to update all of them.
*/
class ParticleWorld
{
public:
	ParticleWorld(unsigned int maxContacts_, unsigned int iterations=0);
	~ParticleWorld();

	unsigned int GenerateContacts();
	void PhysicsUpdate(gFloat dt);

	void AddParticle(Particle* p);
	void AddForceGenerator(ParticleForceGenerator* pfg);
	void AddContactGenerator(ParticleContactGenerator* pcg);

	std::vector<Particle*>& GetParticles();
	std::map<int, ParticleForceGenerator*>& GetForceGenerators();
	std::vector<ParticleContactGenerator*>& GetContactGenerators();

protected:
	// List of all Particles that exist
	std::vector<Particle*> particles;

	// Map of all Force Generators and their unique IDs
	std::map<int, ParticleForceGenerator*> forceGenerators;

	// List of all Contact Generators
	std::vector<ParticleContactGenerator*> contactGenerators;

	// Contact Resolver that calculates and resolves all contacts each frame
	ParticleContactResolver contactResolver;

	// List of all contacts that occured and are processed each frame
	ParticleContact* contacts;

	bool calculateIterations;
	unsigned int maxContacts;

	// Engine/Game can run on variable framerate, but physics runs at a fixed rate
	// This tracks time as it passes to update physics prorperly at fixed steps
	gFloat timeAccumulator;

	gFloat timer;
};
}	// namespace
#endif	// GLADE_PARTICLE_WORLD_H

