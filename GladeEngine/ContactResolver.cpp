#include "ContactResolver.h"

using namespace Glade;

ContactResolver::ContactResolver(unsigned int iter, gFloat impEp, gFloat penEp) : impulseIterations(iter), penetrationIterations(iter), impulseEpsilon(impEp), penetrationEpsilon(penEp) { }
ContactResolver::ContactResolver(unsigned int iIter, unsigned int pIter, gFloat impEp, gFloat penEp) : impulseIterations(iIter), penetrationIterations(pIter), impulseEpsilon(impEp), penetrationEpsilon(penEp) { }

void ContactResolver::SetIterations(unsigned int iter) { impulseIterations = penetrationIterations = iter; }
void ContactResolver::SetIterations(unsigned int iIter, unsigned int pIter) { impulseIterations = iIter; penetrationIterations = pIter; }
void ContactResolver::SetEpsilon(gFloat impEp, gFloat penEp) { impulseEpsilon= impEp; penetrationEpsilon = penEp; }

void ContactResolver::ResolveContacts(Contact* contactArray, unsigned int numContacts)
{
	// Make sure we have things to actually do
	if(numContacts == 0) return;
	if(!IsValid()) return;

	// Prepare Contacts for processing
	for(unsigned int i = 0; i < numContacts; ++i)
		contactArray[i].CalculateInternals();

	// Resolve interpenetration 
	ResolveInterpenetration(contactArray, numContacts);

	// Resolve Velocity
	ResolveImpulse(contactArray, numContacts);
}


void ContactResolver::ResolveImpulse(Contact* contactArray, unsigned int numContacts)
{
	Vector velocityChange[2], angularVelocityChange[2];
	Vector deltaVel;
	gFloat max;
	unsigned int index, i;

	// Iteratively handle Contacts in order of severity
	impulseIterationsUsed = 0;
	while(impulseIterationsUsed < impulseIterations)
	{
		// Find Contact with maximum magnitude of probably velocity change
		max = -impulseEpsilon;
		index = numContacts;
		for(i = 0; i < numContacts; ++i)
		{
			if(contactArray[i].desiredDeltaVel < max)
			{
				max = contactArray[i].desiredDeltaVel;
				index = i;
			}
		}
		
		if(index == numContacts) break;

		// Match awake state at Contact
		contactArray[index].MatchAwakeState();

		// Do resolution on selected Contact
		contactArray[index].ResolveImpulse(velocityChange, angularVelocityChange);
		
		
		// Update the relative/closing velocities of other Contacts with the
		// same body(s) as the selected Contact using the saved/returned 
		// velocity and angular velocity changes
		for(i = 0; i < numContacts; ++i)
		{
		//	if(i == index) continue;
			if(contactArray[index].b1 == contactArray[i].b1)
			{
				deltaVel = velocityChange[0] + 
					angularVelocityChange[0].CrossProduct(contactArray[i].b1ContactPoint);
				contactArray[i].relativeVelocity += contactArray[i].contactToWorld.MultiplyInverse3(deltaVel);
				contactArray[i].CalculateDesiredDeltaVelocity();
			}
			else if(contactArray[index].b1 == contactArray[i].b2)
			{
				deltaVel = velocityChange[0] + 
					angularVelocityChange[0].CrossProduct(contactArray[i].b2ContactPoint);
				contactArray[i].relativeVelocity -= contactArray[i].contactToWorld.MultiplyInverse3(deltaVel);
				contactArray[i].CalculateDesiredDeltaVelocity();
			}

			if(contactArray[index].b2 != nullptr)
			{
				if(contactArray[index].b2 == contactArray[i].b1)
				{
					deltaVel = velocityChange[1] +
						angularVelocityChange[1].CrossProduct(contactArray[i].b1ContactPoint);
					contactArray[i].relativeVelocity += contactArray[i].contactToWorld.MultiplyInverse3(deltaVel);
					contactArray[i].CalculateDesiredDeltaVelocity();
				}
				else if(contactArray[index].b2 == contactArray[i].b2)
				{
					deltaVel = velocityChange[1] + 
						angularVelocityChange[1].CrossProduct(contactArray[i].b2ContactPoint);
					contactArray[i].relativeVelocity -= contactArray[i].contactToWorld.MultiplyInverse3(deltaVel);
					contactArray[i].CalculateDesiredDeltaVelocity();
				}
			}
		}

		++impulseIterationsUsed;
	}
}

void ContactResolver::ResolveInterpenetration(Contact* contactArray, unsigned int numContacts)
{
	Vector linearChange[2], angularChange[2];
	Vector deltaPos;
	gFloat max;
	unsigned int index, i;

	// Iteratively handle Contacts in order of severity
	penetrationIterationsUsed = 0;
	while(penetrationIterationsUsed < penetrationIterations)
	{
		// Find largest interpenetration
		max = penetrationEpsilon;
		index = numContacts;
		for(i = 0; i < numContacts; ++i)
		{
			if(contactArray[i].penetrationDepth > max)
			{
				max = contactArray[i].penetrationDepth;
				index = i;
			}
		}
		if(index == numContacts) break;

		// Match awake state at Contact
		contactArray[index].MatchAwakeState();

		// Do resolution on selected Contact
		contactArray[index].ResolveInterpenetration(linearChange, angularChange, max);
		
		// Update the interpenetration of other Contacts with the
		// same body(s) as the selected Contact using the saved/returned 
		// linear and angular changes
		for(i = 0; i < numContacts; ++i)
		{
		//	if(i == index) continue;
			if(contactArray[index].b1 == contactArray[i].b1)
			{
				deltaPos = linearChange[0] + 
					angularChange[0].CrossProduct(contactArray[i].b1ContactPoint);
				contactArray[i].penetrationDepth += deltaPos.DotProduct(contactArray[i].normal);
			}
			else if(contactArray[index].b1 == contactArray[i].b2)
			{
				deltaPos = linearChange[0] + 
					angularChange[0].CrossProduct(contactArray[i].b2ContactPoint);
				contactArray[i].penetrationDepth -= deltaPos.DotProduct(contactArray[i].normal);
			}

			if(contactArray[index].b2 != nullptr)
			{
				if(contactArray[index].b2 == contactArray[i].b1)
				{
					deltaPos = linearChange[1] +
						angularChange[1].CrossProduct(contactArray[i].b1ContactPoint);
					contactArray[i].penetrationDepth += deltaPos.DotProduct(contactArray[i].normal);
				}
				else if(contactArray[index].b2 == contactArray[i].b2)
				{
					deltaPos = linearChange[1] + 
						angularChange[1].CrossProduct(contactArray[i].b2ContactPoint);
					contactArray[i].penetrationDepth -= deltaPos.DotProduct(contactArray[i].normal);
				}
			}
		}
		++penetrationIterationsUsed;
	}
}