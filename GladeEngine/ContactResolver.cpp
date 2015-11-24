#include "ContactResolver.h"

using namespace Glade;

ContactResolver::ContactResolver(unsigned int iter, gFloat impEp, gFloat penEp) : impulseIterations(iter), penetrationIterations(iter), impulseEpsilon(impEp), penetrationEpsilon(penEp) { }
ContactResolver::ContactResolver(unsigned int iIter, unsigned int pIter, gFloat impEp, gFloat penEp) : impulseIterations(iIter), penetrationIterations(pIter), impulseEpsilon(impEp), penetrationEpsilon(penEp) { }

void ContactResolver::SetIterations(unsigned int iter) { impulseIterations = penetrationIterations = iter; }
void ContactResolver::SetIterations(unsigned int iIter, unsigned int pIter) { impulseIterations = iIter; penetrationIterations = pIter; }
void ContactResolver::SetEpsilon(gFloat impEp, gFloat penEp) { impulseEpsilon= impEp; penetrationEpsilon = penEp; }

void ContactResolver::ResolveContacts(ContactBatch* contactBatch)
{
	// Make sure we have things to actually do
	if(!IsValid()) return;

	// Prepare Contacts for processing
	contactBatch->CalculateInternals();

	// Resolve interpenetration 
	ResolveInterpenetration(contactBatch->GetHead(), contactBatch->GetNumContacts());

	// Resolve Velocity
	ResolveImpulse(contactBatch->GetHead(), contactBatch->GetNumContacts());
}


void ContactResolver::ResolveImpulse(ContactBatchNode* contactBatch, unsigned int numContacts)
{
	Vector velocityChange[2], angularVelocityChange[2];
	Vector deltaVel;
	gFloat max;
	unsigned int index, i; 
	ContactBatchNode* selected, *temp;

	// Iteratively handle Contacts in order of severity
	impulseIterationsUsed = 0;
	while(impulseIterationsUsed < impulseIterations)
	{
		// Find Contact with maximum magnitude of probably velocity change
		max = -impulseEpsilon;
		index = numContacts;
		for(i = 0; i < numContacts; ++i)
		{
			if(contactBatch->contact.desiredDeltaVel < max)
			{
				max = contactBatch->contact.desiredDeltaVel;
				selected = contactBatch;
				index = i;
			}

			// Move to next Contact in Batch
			contactBatch = contactBatch->GetNext();
		}
		
		if(index == numContacts) break;

		// Match awake state at Contact
		selected->contact.MatchAwakeState();

		// Do resolution on selected Contact
		selected->contact.ResolveImpulse(velocityChange, angularVelocityChange);
		
		// Update the relative/closing velocities of other Contacts with the
		// same body(s) as the selected Contact using the saved/returned 
		// velocity and angular velocity changes
		temp = contactBatch;	// loop through Batch with new pointer
   		for(i = 0; i < numContacts; ++i)
		{
			if(selected->contact.b1 == temp->contact.b1)
			{
				deltaVel = velocityChange[0] + 
					//angularVelocityChange[0].CrossProduct(contactArray[i].b1ContactPoint);
					temp->contact.b1ContactPoint.CrossProduct(angularVelocityChange[0]);
				temp->contact.relativeVelocity += temp->contact.contactToWorld.MultiplyInverse3(deltaVel);
				temp->contact.CalculateDesiredDeltaVelocity();
			}
			else if(selected->contact.b1 == temp->contact.b2)
			{
				deltaVel = velocityChange[0] + 
					//angularVelocityChange[0].CrossProduct(contactArray[i].b2ContactPoint);
					temp->contact.b2ContactPoint.CrossProduct(angularVelocityChange[0]);
				temp->contact.relativeVelocity -= temp->contact.contactToWorld.MultiplyInverse3(deltaVel);
				temp->contact.CalculateDesiredDeltaVelocity();
			}

			if(selected->contact.b2 != nullptr)
			{
				if(selected->contact.b2 == temp->contact.b1)
				{
					deltaVel = velocityChange[1] +
						//angularVelocityChange[1].CrossProduct(contactArray[i].b1ContactPoint);
						temp->contact.b1ContactPoint.CrossProduct(angularVelocityChange[1]);
					temp->contact.relativeVelocity += temp->contact.contactToWorld.MultiplyInverse3(deltaVel);
					temp->contact.CalculateDesiredDeltaVelocity();
				}
				else if(selected->contact.b2 == temp->contact.b2)
				{
					deltaVel = velocityChange[1] + 
						//angularVelocityChange[1].CrossProduct(contactArray[i].b2ContactPoint);
						temp->contact.b2ContactPoint.CrossProduct(angularVelocityChange[1]);
					temp->contact.relativeVelocity -= temp->contact.contactToWorld.MultiplyInverse3(deltaVel);
					temp->contact.CalculateDesiredDeltaVelocity();
				}
			}
			temp = temp->GetNext();
		}

		++impulseIterationsUsed;
	}
}

void ContactResolver::ResolveInterpenetration(ContactBatchNode* contactBatch, unsigned int numContacts)
{
	Vector linearChange[2], angularChange[2];
	Vector deltaPos;
	gFloat max;
	unsigned int index, i;
	ContactBatchNode* selected, *temp;

	// Iteratively handle Contacts in order of severity
	penetrationIterationsUsed = 0;
	while(penetrationIterationsUsed < penetrationIterations)
	{
		// Find largest interpenetration
		max = penetrationEpsilon;
		index = numContacts;
		for(i = 0; i < numContacts; ++i)
		{
			if(contactBatch->contact.penetrationDepth > max)
			{
				max = contactBatch->contact.penetrationDepth;
				selected = contactBatch;
				index = i;
			}
		}
		if(index == numContacts) break;

		// Match awake state at Contact
		selected->contact.MatchAwakeState();

		// Do resolution on selected Contact
		selected->contact.ResolveInterpenetration(linearChange, angularChange, max);
		
		// Update the interpenetration of other Contacts with the
		// same body(s) as the selected Contact using the saved/returned 
		// linear and angular changes
		temp = contactBatch;	// loop through Batch with new pointer
		for(i = 0; i < numContacts; ++i)
		{
			if(selected->contact.b1 == temp->contact.b1)
			{
				deltaPos = linearChange[0] + 
					//angularChange[0].CrossProduct(contactArray[i].b1ContactPoint);
					temp->contact.b1ContactPoint.CrossProduct(angularChange[0]);
				temp->contact.penetrationDepth += deltaPos.DotProduct(temp->contact.normal);
			}
			else if(selected->contact.b1 == temp->contact.b2)
			{
				deltaPos = linearChange[0] + 
					//angularChange[0].CrossProduct(contactArray[i].b2ContactPoint);
					temp->contact.b2ContactPoint.CrossProduct(angularChange[0]);
				temp->contact.penetrationDepth -= deltaPos.DotProduct(temp->contact.normal);
			}

			if(selected->contact.b2 != nullptr)
			{
				if(selected->contact.b2 == temp->contact.b1)
				{
					deltaPos = linearChange[1] +
						//angularChange[1].CrossProduct(contactArray[i].b1ContactPoint);
						temp->contact.b1ContactPoint.CrossProduct(angularChange[1]);
					temp->contact.penetrationDepth += deltaPos.DotProduct(temp->contact.normal);
				}
				else if(selected->contact.b2 == temp->contact.b2)
				{
					deltaPos = linearChange[1] + 
						//angularChange[1].CrossProduct(contactArray[i].b2ContactPoint);
						temp->contact.b2ContactPoint.CrossProduct(angularChange[1]);
					temp->contact.penetrationDepth -= deltaPos.DotProduct(temp->contact.normal);
				}
			}
			temp = temp->GetNext();
		}
		++penetrationIterationsUsed;
	}
}