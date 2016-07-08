#include "ContactResolver.h"
#include "../Utils/Trace.h"
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
#ifdef SOLVE_PENETRATION_SIMULTANEOUS
//	ResolveInterpenetration4(contactBatch, contactBatch->GetNumContacts());
	ResolveInterpenetration3(contactBatch, contactBatch->GetNumContacts());
//	ResolveInterpenetration2(contactBatch->GetHead(), contactBatch->GetNumContacts());
#else
	ResolveInterpenetration(contactBatch->GetHead(), contactBatch->GetNumContacts());
#endif

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
		// Find Contact with maximum magnitude of probable velocity change
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
					angularVelocityChange[0].CrossProduct(temp->contact.b1ContactPoint);
					//temp->contact.b1ContactPoint.CrossProduct(angularVelocityChange[0]);
				temp->contact.relativeVelocity += temp->contact.contactToWorld.Transpose3Times(deltaVel);
				temp->contact.CalculateDesiredDeltaVelocity();
			}
			else if(selected->contact.b1 == temp->contact.b2)
			{
				deltaVel = velocityChange[0] + 
					angularVelocityChange[0].CrossProduct(temp->contact.b2ContactPoint);
					//temp->contact.b2ContactPoint.CrossProduct(angularVelocityChange[0]);
				temp->contact.relativeVelocity -= temp->contact.contactToWorld.Transpose3Times(deltaVel);
				temp->contact.CalculateDesiredDeltaVelocity();
			}

			if(selected->contact.b2 != nullptr)
			{
				if(selected->contact.b2 == temp->contact.b1)
				{
					deltaVel = velocityChange[1] +
						angularVelocityChange[1].CrossProduct(temp->contact.b1ContactPoint);
						//temp->contact.b1ContactPoint.CrossProduct(angularVelocityChange[1]);
					temp->contact.relativeVelocity += temp->contact.contactToWorld.Transpose3Times(deltaVel);
					temp->contact.CalculateDesiredDeltaVelocity();
				}
				else if(selected->contact.b2 == temp->contact.b2)
				{
					deltaVel = velocityChange[1] + 
						angularVelocityChange[1].CrossProduct(temp->contact.b2ContactPoint);
						//temp->contact.b2ContactPoint.CrossProduct(angularVelocityChange[1]);
					temp->contact.relativeVelocity -= temp->contact.contactToWorld.Transpose3Times(deltaVel);
					temp->contact.CalculateDesiredDeltaVelocity();
				}
			}
			temp = temp->GetNext();
		}

		++impulseIterationsUsed;
	}
}

#ifndef SOLVE_PENETRATION_SIMULTANEOUS
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

			// Move to next Contact in Batch
			contactBatch = contactBatch->GetNext();
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
					angularChange[0].CrossProduct(temp->contact.b1ContactPoint);
					//temp->contact.b1ContactPoint.CrossProduct(angularChange[0]);
				temp->contact.penetrationDepth += deltaPos.DotProduct(temp->contact.normal);
			}
			else if(selected->contact.b1 == temp->contact.b2)
			{
				deltaPos = linearChange[0] + 
					angularChange[0].CrossProduct(temp->contact.b2ContactPoint);
					//temp->contact.b2ContactPoint.CrossProduct(angularChange[0]);
				temp->contact.penetrationDepth -= deltaPos.DotProduct(temp->contact.normal);
			}

			if(selected->contact.b2 != nullptr)
			{
				if(selected->contact.b2 == temp->contact.b1)
				{
					deltaPos = linearChange[1] +
						angularChange[1].CrossProduct(temp->contact.b1ContactPoint);
						//temp->contact.b1ContactPoint.CrossProduct(angularChange[1]);
					temp->contact.penetrationDepth += deltaPos.DotProduct(temp->contact.normal);
				}
				else if(selected->contact.b2 == temp->contact.b2)
				{
					deltaPos = linearChange[1] + 
						angularChange[1].CrossProduct(temp->contact.b2ContactPoint);
						//temp->contact.b2ContactPoint.CrossProduct(angularChange[1]);
					temp->contact.penetrationDepth -= deltaPos.DotProduct(temp->contact.normal);
				}
			}
			temp = temp->GetNext();
		}
		++penetrationIterationsUsed;
	}
}
#endif

#ifdef SOLVE_PENETRATION_SIMULTANEOUS
// INTERPENETRATION NEEDS TO BE SOLVED AS A COMPLEX SYSTEM OF EQUATIONS
// SO ALL PENETRATIONS ARE RESOLVED AT ONCE
// LOTS OF WORK! BARAFF TYPE OF SHIT

/*
	Calc Resolution for current contact
	Loop through previous contacts backwards - test penetration after new calc resolution
	Add additional resolution to previous contacts until new penetration is 0
	Repeat through all contacts
*//*
void ContactResolver::ResolveInterpenetration2(ContactBatchNode* contactBatch, unsigned int numContacts)
{
	Vector deltaPos[2];
	Vector deltaOrient[2];
	gFloat* currentPenetrations = new gFloat[numContacts];
	ContactBatchNode* current=contactBatch, *temp;
	gFloat totalPen;

	// Resolutions need to be tracked per RigidBody, not per Contact
	struct PenResolution { Vector deltaPos; Vector deltaOrient; };
	std::map<RigidBody*, PenResolution> resolutions;
	for(unsigned int i = 0; i < numContacts; ++i)
	{
		resolutions.insert(std::pair<RigidBody*, PenResolution>(current->contact.b1, PenResolution()));
		resolutions.insert(std::pair<RigidBody*, PenResolution>(current->contact.b2, PenResolution()));
		current = current->GetNext();
	}

	// Calc Resolution for 1st Contact in Batch
	contactBatch->contact.CalculateInertia();
	contactBatch->contact.CalculatePenetrationResolution(deltaPos, deltaOrient, contactBatch->contact.penetrationDepth);
	resolutions[contactBatch->contact.b1].deltaPos += deltaPos[0];
	resolutions[contactBatch->contact.b1].deltaOrient += deltaOrient[0];
	resolutions[contactBatch->contact.b2].deltaPos += deltaPos[1];
	resolutions[contactBatch->contact.b2].deltaOrient += deltaOrient[1];
	for(unsigned int i = 1; i < numContacts; ++i)
	{
		// Calc Resolution for next Contact in Batch
		current = contactBatch->next;
		current->contact.CalculateInertia();
		current->contact.CalculatePenetrationResolution(deltaPos, deltaOrient, current->contact.penetrationDepth);
		resolutions[current->contact.b1].deltaPos += deltaPos[0];
		resolutions[current->contact.b1].deltaOrient += deltaOrient[0];
		resolutions[current->contact.b2].deltaPos += deltaPos[1];
		resolutions[current->contact.b2].deltaOrient += deltaOrient[1];

		// Calculate all penetrations given current resolutions
		temp = contactBatch;
		totalPen = 0.0f;
		for(unsigned int j = 0; j <= i; ++j)
		{
			currentPenetrations[j] = temp->contact.penetrationDepth
					+ (resolutions[temp->contact.b1].deltaPos + resolutions[temp->contact.b1].deltaOrient.CrossProduct(temp->contact.b1ContactPoint)).DotProduct(temp->contact.normal)
					- (resolutions[temp->contact.b2].deltaPos + resolutions[temp->contact.b2].deltaOrient.CrossProduct(temp->contact.b2ContactPoint)).DotProduct(temp->contact.normal);
			temp = temp->GetNext();
			totalPen += Abs(currentPenetrations[j]);
		}

		// Test current resolutions to see if they all resolve to 0 penetration
		if(totalPen <= (penetrationEpsilon * (i+1)))
			continue;

		// Resolutions do not resolve to 0 penetration in all Contacts
		// Look like we got some work to do


		// Search through each previous Contact (that is penetrating after current Contacts resolution) to see if (chained together) they eventually share one of the current Contact's RigidBodies
		// Apply said RigidBody's resolution to all bodies in that chain
		// (Checking only for Contacts that are still penetrating should eliminate issue of Contacts penetrating perpendicular to current Contact resolving in the wrong direction
		// (i.e. a box lying next to the bottom box on the platform being pushing up because Contact 2's resoluton was added to it)
		//
		// OR
		//
		// Figure out what to pass to contact->CalculatePenetrationResolution to resolve all contacts together

		std::set<Contact*> updatedContacts;
		std::set<RigidBody*> sharedBodies;
		sharedBodies.insert(current->contact.b1);
		sharedBodies.insert(current->contact.b2);
		temp = current->GetPrevious();
		for(int j = i-1; j >= 0; --j)	// Loop through all previous Contacts
		{
			if(currentPenetrations[j] <= gFloat(0.0f) ||
				updatedContacts.count(&temp->contact) > 0) continue;

			// Find all Contacts chained together (by their RigidBodies) that are no longer resolved after current Contact resoluton
			if(sharedBodies.count(temp->contact.b1) > 0)
			{
				sharedBodies.insert(temp->contact.b2);	// Add other body to list so we can chain off of this contact

				// Update Contact with resolution from current Contact
				unsigned int k = temp->contact.b1 == current->contact.b1 ? 0 : 1;
				resolutions[temp->contact.b1].deltaPos += deltaPos[k] * gFloat(0.5f);
				resolutions[temp->contact.b1].deltaOrient += deltaOrient[k] * gFloat(0.5f);
				resolutions[temp->contact.b2].deltaPos += deltaPos[k] * gFloat(1.5f);
				resolutions[temp->contact.b2].deltaOrient += deltaOrient[k] * gFloat(1.5f);

				// We need to restart the search because we found a new Contact, and therefore a new RigidBody, in the chain
				// Now we need to double-check that we didn't skip a Contact that wasn't connected in the chain until this one was added
				updatedContacts.insert(&temp->contact);
				temp = current;
				j = i;
			}
			else if(sharedBodies.count(temp->contact.b2) > 0)
			{
				sharedBodies.insert(temp->contact.b1);	// Add other body to list so we can chain off of this contact

				// Update Contact with resolution from current Contact
				unsigned int k = temp->contact.b2 == current->contact.b1 ? 0 : 1;
				resolutions[temp->contact.b1].deltaPos += deltaPos[k] * gFloat(1.5f);
				resolutions[temp->contact.b1].deltaOrient += deltaOrient[k] * gFloat(1.5f);
				resolutions[temp->contact.b2].deltaPos += deltaPos[k] * gFloat(0.5f);
				resolutions[temp->contact.b2].deltaOrient += deltaOrient[k] * gFloat(0.5f);

				// We need to restart the search because we found a new Contact, and therefore a new RigidBody, in the chain
				// Now we need to double-check that we didn't skip a Contact that wasn't connected in the chain until this one was added
				updatedContacts.insert(&temp->contact);
				temp = current;
				j = i;
			}

			temp = temp->GetPrevious();
		}

		temp = contactBatch;
		totalPen = 0.0f;
		for(unsigned int j = 0; j <= i; ++j)
		{
			currentPenetrations[j] = temp->contact.penetrationDepth
					+ (resolutions[temp->contact.b1].deltaPos + resolutions[temp->contact.b1].deltaOrient.CrossProduct(temp->contact.b1ContactPoint)).DotProduct(temp->contact.normal)
					- (resolutions[temp->contact.b2].deltaPos + resolutions[temp->contact.b2].deltaOrient.CrossProduct(temp->contact.b2ContactPoint)).DotProduct(temp->contact.normal);
			temp = temp->GetNext();
			totalPen += Abs(currentPenetrations[j]);
		}
	}

	// ~~~~ END ~~~~
	// Apply all resolutions now that they have been calculated
	for(auto iter = resolutions.begin(); iter != resolutions.end(); ++iter)
	{
		iter->first->ForceAddPosition(iter->second.deltaPos);
		iter->first->ForceAddOrientation(iter->second.deltaOrient);
	}
}
*/

/*
	REDO EVERYTHING
	Order/Organize Contacts by finding most 'major' Contact 
									Infinite Mass Body > Highest Relative Velocity > Highest/Lowest Body > Arbitrary
	Track Contacts Relative to each other
	Solve most major Contact
	Solve related Contacts, adjust assuming most major won't change
	????
	Profit
*/
//#define IGNORE_ROTATION
void ContactResolver::ResolveInterpenetration3(ContactBatch* contactBatch, unsigned int numContacts)
{
	Vector deltaPos[2];
	Vector deltaOrient[2];
	gFloat* currentPenetrations = new gFloat[numContacts];
	ContactBatchNode* major = contactBatch->major, *temp;
	RigidBody* b1 = major->contact.b1, *b2 = major->contact.b2;	// save "most major" bodies for simplicity

	// Resolutions need to be tracked per RigidBody, not per Contact
	// Track the resolution per RigidBody with this
	struct PenResolution { Vector deltaPos; Vector deltaOrient; };
	std::map<RigidBody*, PenResolution> resolutions;
	for(auto iter = contactBatch->nodes.begin(); iter != contactBatch->nodes.end(); ++iter)
	{
		resolutions.insert(std::pair<RigidBody*, PenResolution>((*iter)->contact.b1, PenResolution()));
		resolutions.insert(std::pair<RigidBody*, PenResolution>((*iter)->contact.b2, PenResolution()));
	}

	// Match awake state at major Contact
	major->contact.MatchAwakeState();

	// Calculate resolution for "most major" Contact
	major->contact.CalculateInertia();
	major->contact.CalculatePenetrationResolution(deltaPos, deltaOrient, major->contact.penetrationDepth);

	// Save calculated resoltuon for "most major" Contact
	resolutions[major->contact.b1].deltaPos += deltaPos[0];
	resolutions[major->contact.b1].deltaOrient += deltaOrient[0];
	resolutions[major->contact.b2].deltaPos += deltaPos[1];
	resolutions[major->contact.b2].deltaOrient += deltaOrient[1];
	// These are not going to change...EVER!
	if(deltaOrient[0] != Vector() || deltaOrient[1] != Vector())
	{
		int x = 5;
		x++;
	}

	struct ContactGraph
	{ 
		ContactGraph(){} 
		ContactGraph(ContactBatchNode* n, RigidBody* b, Vector norm, Contact* c) : node(n), parentBody(b), parentNormal(norm), parentContact(c) { } 
		ContactBatchNode* node; 
		RigidBody* parentBody; 
		Vector parentNormal;
		Contact* parentContact;
	};

	penetrationIterationsUsed = 0;
	while(penetrationIterationsUsed < 1)
	{
		// Go Left - IGNORING ROTATION FOR NOW
		std::queue<ContactGraph> queue;
		auto list = major->GetLeft();
		for(auto iter = list.begin(); iter != list.end(); ++iter)
			queue.push(ContactGraph(*iter, major->contact.b1, -major->contact.normal, &major->contact));
		// Go Right - STILL DONT'T KNOW WHAT TO DO WITH ROTATION
		list = major->GetRight();
		for(auto iter = list.begin(); iter != list.end(); ++iter)
			queue.push(ContactGraph(*iter, major->contact.b2, major->contact.normal, &major->contact));

		ContactGraph cg;
		while(queue.size() > 0)
		{
			// Grab next Contact off of the queue
			cg = queue.front();
			queue.pop();

			// Add all of its connected Contacts to the queue
			list = cg.node->GetLeft();
			for(auto iter = list.begin(); iter != list.end(); ++iter)
				queue.push(ContactGraph(*iter, cg.node->contact.b1, -cg.node->contact.normal, &cg.node->contact));
			list = cg.node->GetRight();
			for(auto iter = list.begin(); iter != list.end(); ++iter)
				queue.push(ContactGraph(*iter, cg.node->contact.b2, cg.node->contact.normal, &cg.node->contact));

			// Match awake state at Contact
			cg.node->contact.MatchAwakeState();

			// Calculate normal resolution for Contact
			cg.node->contact.CalculateInertia();
			cg.node->contact.CalculatePenetrationResolution(deltaPos, deltaOrient, cg.node->contact.penetrationDepth);

			// MOST MAJOR CANNOT CHANGE IN DIRECTION OF ITS NORMAL
			// BUT IT CAN CHANGE IN NON-NORMAL DIRECTION
			// Update calculated resolution with Resolution from 'parent' Contact 
		/*	if(cg.node->contact.b1 == cg.parentBody)
			{
				deltaPos[1] += -deltaPos[0];
				Vector blah = 
				//cg.node->contact.normal * resolutions[cg.node->contact.b1].deltaPos.DotProduct(-major->contact.normal);
				//cg.normal * resolutions[cg.parentBody].deltaPos.DotProduct(cg.normal);
				resolutions[cg.parentBody].deltaPos * cg.node->contact.normal.DotProduct(cg.normal);
				deltaPos[1] += blah;
				resolutions[cg.node->contact.b2].deltaPos += deltaPos[1];
			}
			else
			{
				deltaPos[0] += -deltaPos[1];
				Vector blah = 
				//cg.node->contact.normal * resolutions[cg.node->contact.b2].deltaPos.DotProduct(major->contact.normal);
				//cg.normal * resolutions[cg.parentBody].deltaPos.DotProduct(cg.normal);
				resolutions[cg.parentBody].deltaPos * cg.node->contact.normal.DotProduct(cg.normal);
				deltaPos[0] += blah;
				resolutions[cg.node->contact.b1].deltaPos += deltaPos[0];
			}
			*/

			// 2nd Contact can resolve in any direction when that direction.DotProduct(previousResolutionDirection) >= 0
			// 2nd Contact cannot resolve in direction when that direction.DotProduct(previousResolutionDirection) < 0

			// Movement from parent Contact carried over onto current Contact
			Vector blah = 
				//cg.normal * resolutions[cg.parentBody].deltaPos.DotProduct(cg.normal);
				resolutions[cg.parentBody].deltaPos * cg.node->contact.normal.DotProduct(cg.parentNormal);

			// If 1st body in Contact is the body shared with parent Contact
			if(cg.node->contact.b1 == cg.parentBody)
			{	// If resolution of shared body in some way goes against the resolution of parent Contact (would make parent Contact penetrate again)
				gFloat dot = deltaPos[0].DotProduct(cg.parentNormal);
				if(dot< gFloat(0.0f))
				{
					// Negate that resolution, push other body out in opposite direction by same amount
					Vector blah2 = cg.parentNormal * dot;	// said resolution
					deltaPos[1] -= blah2;	// add resolution onto other body
					deltaPos[0] -= blah2;	// remove resolution from shared body (it cannot resolve in this direction)
					deltaPos[1] += blah;	// add shared body's resolution from parent Contact to other body
				}

	#ifndef IGNORE_ROTATION
				// If shared body wants to rotate NOT around normal axis
				if(deltaOrient[0].Normalized() != cg.parentNormal)
				{
					Vector dPos[2], dOri[2];
					// Calc resolution that would have been resolved via rotation
					//gFloat pen = deltaOrient[0].CrossProduct(cg.parentContact->b1ContactPoint).DotProduct(cg.node->contact.normal);
					gFloat pen = cg.node->contact.penetrationDepth + blah + deltaPos[0].DotProduct(cg.node->contact.normal) - deltaPos[1].DotProduct(cg.node->contact.normal);


					// If Contact normal is in same direction as parent normal (like boxes in a stack)
					if(Abs(cg.node->contact.normal.DotProduct(cg.parentNormal)) >= gFloat(0.1f))
					{	// Recalc resolutions for penetration missing from rotation
						cg.node->contact.CalculatePenetrationResolution(dPos, dOri, pen);

						// Apply
					//	deltaPos[1] += cg.node->contact.normal * pen;
					//	deltaOrient[0] = cg.parentNormal * deltaOrient[0].DotProduct(cg.parentNormal);
					//	deltaOrient[1] = cg.parentNormal * deltaOrient[1].DotProduct(cg.parentNormal);
					//	6/5/16

						deltaPos[1] += cg.node->contact.normal * pen;
						deltaOrient[0].Zero();
						deltaOrient[1].Zero();
					}
					else	// Contact normal is not in same direction as parent normal
					{
						// Forcibly add that resolution
						deltaPos[0] += cg.node->contact.normal * -pen;

						// Remove orientation not along normal axis
						deltaOrient[0] = cg.parentNormal * deltaOrient[0].DotProduct(cg.parentNormal);
					}
				}
	#endif
			}	// If 2nd body in Contact is body shared by parent Contact
			else if(cg.node->contact.b2 == cg.parentBody)
			{
				if(deltaPos[1].DotProduct(cg.parentNormal) < gFloat(0.0f))
				{
					Vector blah2 = cg.parentNormal * cg.parentNormal.DotProduct(deltaPos[1]);
					deltaPos[1] -= blah2;
					deltaPos[0] -= blah2;
					deltaPos[0] += blah;
				}

	#ifndef IGNORE_ROTATION
				if(deltaOrient[1].Normalized() != cg.parentNormal)
				{
					// Calc resolution that would have been resolved via rotation
				//	gFloat pen = deltaOrient[1].CrossProduct(cg.parentContact->b1ContactPoint).DotProduct(cg.normal);
				//	gFloat pen2 = deltaOrient[1].CrossProduct(cg.node->contact.b2ContactPoint).DotProduct(cg.node->contact.normal);
					gFloat pen = cg.node->contact.penetrationDepth + deltaPos[0].DotProduct(cg.node->contact.normal) - deltaPos[1].DotProduct(cg.node->contact.normal);
				
					if(Abs(cg.node->contact.normal.DotProduct(cg.parentNormal)) == gFloat(1.0f))
					{
						deltaPos[0] -= cg.node->contact.normal * pen;
						//deltaOrient[0] = cg.parentNormal * deltaOrient[0].DotProduct(cg.parentNormal);
						//deltaOrient[1] = cg.parentNormal * deltaOrient[1].DotProduct(cg.parentNormal);
						// 6/5/16
						deltaOrient[0].Zero();
						deltaOrient[1].Zero();
					}
					else
					{
						// Forcibly add that resolution
						deltaPos[1] += cg.node->contact.normal * pen;
				
						// Remove orientation not aong normal axis
						deltaOrient[1] = cg.parentNormal * deltaOrient[1].DotProduct(cg.parentNormal);
					}
				}
	#endif
			}


			gFloat orientPen = cg.node->contact.penetrationDepth
						+ (resolutions[cg.node->contact.b1].deltaPos + resolutions[cg.node->contact.b1].deltaOrient.CrossProduct(cg.node->contact.b1ContactPoint)).DotProduct(cg.node->contact.normal)
						- (resolutions[cg.node->contact.b2].deltaPos + resolutions[cg.node->contact.b2].deltaOrient.CrossProduct(cg.node->contact.b2ContactPoint)).DotProduct(cg.node->contact.normal);
		
			resolutions[cg.node->contact.b1].deltaPos += deltaPos[0];
			resolutions[cg.node->contact.b2].deltaPos += deltaPos[1];
			resolutions[cg.node->contact.b1].deltaOrient += deltaOrient[0];
			resolutions[cg.node->contact.b2].deltaOrient += deltaOrient[1];
			if(deltaOrient[0] != Vector() || deltaOrient[1] != Vector())
			{
				int x = 5;
				x++;
			}
		}

		// ~~~~ END ~~~~
		// Apply all resolutions now that they have been calculated
		for(auto iter = resolutions.begin(); iter != resolutions.end(); ++iter)
		{
			if(iter->first->GetInverseMass() == gFloat(0.0f))
				continue;
			//TRACE("Object %i Linear Move: (%f, %f, %f)\n", iter->first->GetID(), iter->second.deltaPos.x,iter->second.deltaPos.y,iter->second.deltaPos.z);
			TRACE("Object %i Angular Move: (%f, %f, %f)\n", iter->first->GetID(), iter->second.deltaOrient.x,iter->second.deltaOrient.y,iter->second.deltaOrient.z);
			iter->first->ForceAddPosition(iter->second.deltaPos);
			iter->first->ForceAddOrientation(iter->second.deltaOrient);

			// Recalculate derived data for sleeping RigidBodies now that the changes are applied
			//  (Awake bodies will automatically do this after Integration)
			if(!iter->first->GetAwake())
				iter->first->CalcDerivedData();
		}
		penetrationIterationsUsed++;
	}
}

void ContactResolver::ResolveInterpenetration4(ContactBatch* contactBatch, unsigned int numContacts)
{
	Vector deltaPos[2];
	Vector deltaOrient[2];
	gFloat* currentPenetrations = new gFloat[numContacts];
	ContactBatchNode* major = contactBatch->major, *temp = contactBatch->major;
	RigidBody* b1 = major->contact.b1, *b2 = major->contact.b2;	// save "most major" bodies for simplicity


	struct ContactGraph { ContactGraph(){} ContactGraph(ContactBatchNode* n, bool l) : node(n), left(l) {} ContactBatchNode* node; bool left; };
	std::queue<ContactGraph> queue;
	Vector deltaPen;

	// Calculate resolution for "most major" Contact
	temp->contact.CalculateInertia();
	temp->contact.CalculatePenetrationResolution(deltaPos, deltaOrient, major->contact.penetrationDepth);

	// Apply Resolutions
	temp->contact.b1->ForceAddPosition(deltaPos[0]);
	temp->contact.b1->ForceAddOrientation(deltaOrient[0]);
	temp->contact.b2->ForceAddPosition(deltaPos[1]);
	temp->contact.b2->ForceAddOrientation(deltaOrient[1]);
	if(!temp->contact.b1->GetAwake())
		temp->contact.b1->CalcDerivedData();
	if(!temp->contact.b2->GetAwake())
		temp->contact.b2->CalcDerivedData();

	// Go Left
	auto list = temp->GetLeft();
	for(auto iter = list.begin(); iter != list.end(); ++iter)
	{
		deltaPen = deltaPos[0] + 
			deltaOrient[0].CrossProduct((*iter)->contact.b1ContactPoint);
		(*iter)->contact.penetrationDepth += deltaPen.DotProduct((*iter)->contact.normal);

		queue.push(ContactGraph(*iter, true));
	}
	// Go Right
	list = temp->GetRight();
	for(auto iter = list.begin(); iter != list.end(); ++iter)
	{
		deltaPen = deltaPos[1] +
			deltaOrient[1].CrossProduct((*iter)->contact.b2ContactPoint);
		(*iter)->contact.penetrationDepth -= deltaPen.DotProduct((*iter)->contact.normal);

		queue.push(ContactGraph(*iter, false));
	}


	// 2nd Contact can resolve in any direction when that direction.DotProduct(previousResolutionDirection) >= 0
	// 2nd Contact cannot resolve in direction when that direction.DotProduct(previousResolutionDirection) < 0
	ContactGraph cg;
	while(queue.size() > 0)
	{
		cg = queue.front();
		queue.pop();

		cg.node->contact.CalculateInertia();
		if(cg.left)
		{
			cg.node->contact.CalculatePenetrationResolutionB2(deltaPos[1], deltaOrient[1], cg.node->contact.penetrationDepth);

			// Go Right (Left node can never have a left list)
			list = cg.node->GetRight();
			for(auto iter = list.begin(); iter != list.end(); ++iter)
			{
				deltaPen = deltaPos[1] +
					deltaOrient[1].CrossProduct((*iter)->contact.b2ContactPoint);
				(*iter)->contact.penetrationDepth -= deltaPen.DotProduct((*iter)->contact.normal);

				queue.push(ContactGraph(*iter, false));
			}
		}
		else
		{
			cg.node->contact.CalculatePenetrationResolutionB1(deltaPos[0], deltaOrient[0], cg.node->contact.penetrationDepth);

			// Go Left (Right node can never have a right list)
			list = cg.node->GetLeft();
			for(auto iter = list.begin(); iter != list.end(); ++iter)
			{
				deltaPen = deltaPos[0] + 
					deltaOrient[0].CrossProduct((*iter)->contact.b1ContactPoint);
				(*iter)->contact.penetrationDepth += deltaPen.DotProduct((*iter)->contact.normal);

				queue.push(ContactGraph(*iter, true));
			}
		}
	}
}
#endif