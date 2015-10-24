#include "ParticleContactResolver.h"

using namespace Glade;

ParticleContactResolver::ParticleContactResolver(unsigned int iter) : iterations(iter)
{
}


ParticleContactResolver::~ParticleContactResolver()
{
}

void ParticleContactResolver::SetIterations(unsigned int iter)
{
	iterations = iter;
}

void ParticleContactResolver::ResolveContacts(ParticleContact* contactArray, unsigned int numContacts)
{
	unsigned int i, maxIndex=-1;
	gFloat max, closingVel;
	iterationsUsed = 0;

	while(iterationsUsed < iterations)
	{
#ifndef VERLET
		// Find Contact with largest closing velocity
		max = G_MAX;
		maxIndex = numContacts;
		for(i = 0; i < numContacts; ++i)
		{
			closingVel = contactArray[i].CalcRelativeVelocity();
			if(closingVel < max && (closingVel < 0 || contactArray[i].penetrationDepth > 0))
			{
				max = closingVel;
				maxIndex = i;
			}
		}
#else
		// Find Contact with largest interpenetration
		max = 0;
		maxIndex = numContacts;
		for(i = 0; i < numContacts; ++i)
		{
			if(contactArray[i].penetrationDepth > max)
			{
				max = contactArray[i].penetrationDepth;
				maxIndex = i;
			}
		}
#endif

		// Nothing worth resolving
		if(maxIndex == numContacts) break;

		// Match the awake states at the Contact
		contactArray[maxIndex].MatchAwakeState();

		// Resolve Contact with largest closing velocity
		contactArray[maxIndex].Resolve();

		// Update interpenetration for all contacts with particles that were just moved
		for(i = 0; i < numContacts; ++i)
		{
			if(contactArray[i].p1 == contactArray[maxIndex].p1)
				contactArray[i].penetrationDepth -= contactArray[maxIndex].p1Movement.DotProduct(contactArray[i].normal);
			else if(contactArray[i].p1 == contactArray[maxIndex].p2)
				contactArray[i].penetrationDepth -= contactArray[maxIndex].p2Movement.DotProduct(contactArray[i].normal);
			if(contactArray[i].p2)
			{
				if(contactArray[i].p2 == contactArray[maxIndex].p1)
					contactArray[i].penetrationDepth += contactArray[maxIndex].p1Movement.DotProduct(contactArray[i].normal);
				else if(contactArray[i].p2 == contactArray[maxIndex].p2)
					contactArray[i].penetrationDepth += contactArray[maxIndex].p2Movement.DotProduct(contactArray[i].normal);
			}
		}

		// Iteration finished
		iterationsUsed++;
	}
}