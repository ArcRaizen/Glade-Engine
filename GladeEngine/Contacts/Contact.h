#pragma once
#ifndef GLADE_CONTACT_H
#define GLADE_CONTACT_H

#ifndef GLADE_RIGID_BODY_H
#include "RigidBody.h"
#endif

namespace Glade {
// Forward declare ContactResolver class now
class ContactBatch;
class ContactBatchNod;

class Contact
{
public:
	Contact();
	Contact(RigidBody* b1_, RigidBody* b2_, gFloat res, gFloat sMu, gFloat dMu, Vector n, Vector p, gFloat pen);

	void SetNewContact(RigidBody* b1_, RigidBody* b2_, gFloat res, gFloat sMu, gFloat dMu, Vector n, Vector p, gFloat pen);

	friend class ContactResolver;
	friend class ContactBatchNode;
	friend class ContactBatch;

protected:
	void	ResolveImpulse(Vector (&deltaVel)[2], Vector (&deltaAngVel)[2]);
	void	ResolveImpulse2(Vector (&deltaVel)[2], Vector (&deltaAngVel)[2]);

	void	ResolveInterpenetration(Vector (&deltaPos)[2], Vector (&deltaOrient)[2], gFloat pen);
	
	void	CalculateInertia();
	void	CalculatePenetrationResolution(Vector (&deltaPos)[2], Vector (&deltaOrient)[2], gFloat pen);
	void	CalculatePenetrationResolutionB1(Vector& deltaPos, Vector& deltaOrient, gFloat pen);
	void	CalculatePenetrationResolutionB2(Vector& deltaPos, Vector& deltaOrient, gFloat pen);

	// Calculate internal data from state data.
	void	CalculateInternals();

	// Calculate orthonormal basis for Contact Point, using the
	// Contact Normal as one of the principal axes
	void	CalculateContactBasis();

	// Calculate and return velocity of Contact Point on given Rigid Body
	Vector	CalculateLocalVelocity(unsigned int body);

	// Calculate and save desired change in velocity to resolve the Contact
	void	CalculateDesiredDeltaVelocity();

	// Calculate the impulse needed to resolve Contact, assuming no friction.
	Vector	CalcImpulseNoFriction(Matrix (&inverseInertiaTensorWorld)[2]);

	// Calculate the impulse needed to resolve Contact, assuming friction
	Vector	CalcImpulseFriction(Matrix (&inverseInertiaTensorWorld)[2]);

	// Update the awake state of RigidBodies that are involved in this Contact.
	// A RigidBody will be made awake if it is in contact with a RigidBody that is awake
	void	MatchAwakeState();

	// Swap bodies and reverse Contact normal so the Contact is switches direction/perspective
	void	ReverseContact();

private:
	inline bool	HasInfiniteMass() { return b1->GetInverseMass() == gFloat(0.0f) || b2->GetInverseMass() == gFloat(0.0f); }


	RigidBody*	b1;					// First RigidBody in the Contact
	RigidBody*	b2;					// Second RigidBody in the Contact
	gFloat		coeffRestitution;	// Coefficient of Restitution for the Contact
	gFloat		staticFriction;
	gFloat		dynamicFriction;
	Vector		normal;				// Contact normal in world coordinates
	Vector		point;				// Position of Contact Point in world
	gFloat		penetrationDepth;	// Depth of penetration at point of contact


	Matrix		contactToWorld;		// Transform matrix to convert from Contact space to World space
	Vector		relativeVelocity;	// Relative Velocity of colliding bodies at point of Contact
										// Positive means moving toward each other, negative is moving away
	gFloat		desiredDeltaVel;	// Required change in velocity for Contact to be resolved
										// Negative pushes the objects appart, positive pushes together
	Vector		b1ContactPoint;		// World space position of Contact Point relative to b1's Center
	Vector		b2ContactPoint;		// World space position of Contact Point relative to b2's Center

	gFloat		linearInertia[2];
	gFloat		angularInertia[2];
};
}	// namespace
#endif	// GLADE_CONTACT_H
