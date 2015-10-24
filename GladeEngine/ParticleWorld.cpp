#include "ParticleWorld.h"

using namespace Glade;

ParticleWorld::ParticleWorld(unsigned int maxContacts_, unsigned int iterations) : contactResolver(iterations), maxContacts(maxContacts_), timeAccumulator(0.0f)
{
	contacts = new ParticleContact[maxContacts];
//	for(int i = 0; i < maxContacts; i++)
//		contacts[i] = NULL;
	calculateIterations = (iterations==0);
	timer = 0;
}


ParticleWorld::~ParticleWorld()
{
//	for(unsigned int i = 0; i < maxContacts; i++)
//		delete contacts[i];
	delete[] contacts;
}

unsigned int ParticleWorld::GenerateContacts() 
{

	unsigned int limit = maxContacts;
	int used;
	ParticleContact* nextContact = contacts;

	for(auto i = contactGenerators.begin(); i != contactGenerators.end(); ++i)
	{
		used = (*i)->GenerateContact(nextContact, limit);

		if(used < 0)
		{
			delete *i;
			i = contactGenerators.erase(i);
			i--;
			continue;
		}

		limit -= used;
		nextContact += used;

		if(limit <= 0) break;
	}

	return maxContacts - limit;
}

void ParticleWorld::PhysicsUpdate(gFloat dt) 
{
	// Accumulate the time that passed between the last frame and now
	timeAccumulator += dt;
	timer += dt;
	
	// Perform physics update at fixed physics timestep
	while(timeAccumulator >= PHYSICS_TIMESTEP)
	{
	//	timer += PHYSICS_TIMESTEP;
		if(timer >= 5.0f)
			timer = 0.0f;

		// Apply Force Generators and Integrate
		for(auto i = particles.begin(); i != particles.end(); ++i)
		{
			if(timer < 0.05f)
			{
				auto ids = (*i)->GetRegisteredForceGenerators();
				for(auto j = ids.begin(); j != ids.end(); ++j)
					forceGenerators[*j]->GenerateForce(*i);
			}

			// Integrate
			(*i)->Update();
		}

		// Generate and process (if necessary) contacts
		unsigned int usedContacts = GenerateContacts();
		if(usedContacts)
		{
			if(calculateIterations)
				contactResolver.SetIterations(usedContacts*3);
			contactResolver.ResolveContacts(contacts, usedContacts);
		}

		// Now we have spent one frame of time
		timeAccumulator -= PHYSICS_TIMESTEP;
	}
}

void ParticleWorld::AddParticle(Particle* p)
{
	particles.push_back(p);
}

void ParticleWorld::AddForceGenerator(ParticleForceGenerator* pfg)
{
	forceGenerators[pfg->GetID()] = pfg;
}

void ParticleWorld::AddContactGenerator(ParticleContactGenerator* pcg)
{
	contactGenerators.push_back(pcg);
}

std::vector<Particle*>& ParticleWorld::GetParticles()
{
	return particles;
}

std::map<int, ParticleForceGenerator*>& ParticleWorld::GetForceGenerators()
{
	return forceGenerators;
}

std::vector<ParticleContactGenerator*>& ParticleWorld::GetContactGenerators()
{
	return contactGenerators;
}