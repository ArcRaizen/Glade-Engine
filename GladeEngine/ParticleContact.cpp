#include "ParticleContact.h"

using namespace Glade;

ParticleContact::ParticleContact() : p1(nullptr), p2(nullptr), coeffRestitution(0.0f), penetrationDepth(0.0f) {}
ParticleContact::ParticleContact(Particle* p1_, Particle* p2_, gFloat res, Vector n, gFloat pen) : p1(p1_), p2(p2_), coeffRestitution(res), normal(n), penetrationDepth(pen)
{
}


ParticleContact::~ParticleContact() { }

void ParticleContact::SetNewContact(Particle* p1_, Particle* p2_, gFloat res, Vector n, gFloat pen)
{
	p1 = p1_;
	p2 = p2_;
	coeffRestitution = res;
	normal = n;
	penetrationDepth = pen;
}

void ParticleContact::Resolve()
{
#ifndef VERLET
	ResolveImpulse();
#endif
	
	// No penetration? Nothing to do here
	if(penetrationDepth < 0) return;
	ResolveInterpenetration();
#ifdef VERLET
	ResolveImpulseVerlet();
#endif
}

void ParticleContact::MatchAwakeState()
{
	// Collision with world will never cause a Particle to wake up
	if(p2 == nullptr) return;

	bool p1Awake = p1->GetAwake();
	bool p2Awake = p2->GetAwake();

	// Wake up the sleeping one
	if(p1Awake ^ p2Awake)
	{
		if(p1Awake)	p2->SetAwake();
		else		p1->SetAwake();
	}
}

gFloat ParticleContact::CalcRelativeVelocity()
{
#ifndef VERLET
	if(p1->velocityModified || (p2!=nullptr && p2->velocityModified))
#endif
	{
		relativeV = p1->GetVelocity();
		if(p2 != nullptr)
			relativeV -= p2->GetVelocity();
		rel = relativeV.DotProduct(normal);
	}

	return rel;
}

#ifndef VERLET
void ParticleContact::ResolveImpulse()
{
	// Get Relative Velocity of the particles
	gFloat relativeVelocity = CalcRelativeVelocity();

	// Particle(s) is/are either separating or stationary
	if(relativeVelocity > 0) return;

	// Apply change in velocity to each particle proportional to its inverseMass
	gFloat totalInverseMass = p1->GetInverseMass() + (p2 == nullptr ? 0 : p2->GetInverseMass());
	if(totalInverseMass <= 0) return;

	// Calc Relative Velocity post-collision
	gFloat newRelativeVelocity = -relativeVelocity * coeffRestitution;

	// Check velocity buildup compared to this frame's acceleration
	Vector velCauseByAccel = p1->GetAcceleration();
	if(p2 != nullptr)
		velCauseByAccel -= p2->GetAcceleration();
	gFloat relVelCauseByAccel = velCauseByAccel.DotProduct(normal) * HALF_PHYSICS_TIMESTEP_SQR;

	if(relVelCauseByAccel < 0)
	{
		newRelativeVelocity += coeffRestitution * relVelCauseByAccel;

		if(newRelativeVelocity < 0)
			newRelativeVelocity = 0;
	}

	// Calculate & Apply Impulse 
		//	(dV = newRelativeVelocity-relativeVelocity; impulse = dV / totalInverseMass)
	Vector impulsePerIMass = normal * ((newRelativeVelocity - relativeVelocity) / totalInverseMass);
	p1->ForceSetVelocity(p1->GetVelocity() + (impulsePerIMass * p1->GetInverseMass()));

	if(p2 != nullptr)
		p2->ForceSetVelocity(p2->GetVelocity() - (impulsePerIMass * p2->GetInverseMass()));
}
#endif

/*
With Verlet integration not track velocity, a system can lose tons of energy during collision response.
Lets say a ball at (0,5,0) moving (0,-1,0) velocity collides with a wall in a perfectly elastic collision.
Interpenetration resolution moves the ball up slightly (0,0.5,0). The balls previous position was (0,6,0)
and now its current position is (0,5.5,0), cutting its velocity in half, but the ball is also still moving 
down even though it should reflect off the wall. This continues until the velocity reaches 0 and the ball
stops moving. 

DO I CARE ABOUT RELATIVE VELOCITY AND CALCRELATIVEVELOCITY?!?!?!
*/
#ifdef VERLET
void ParticleContact::ResolveImpulseVerlet()
{
	Vector p1Vel = p1->GetVelocity();
	if(coeffRestitution != 0 && !p1Vel.IsZero())
		p1->ForceSetVelocity((p1Vel - (normal * ((gFloat)2.0f * p1Vel.DotProduct(normal)))) * coeffRestitution);

	if(p2 != nullptr)
	{
		Vector p2Vel = p2->GetVelocity();
		if(coeffRestitution != 0 && !p2Vel.IsZero())
			p2->ForceSetVelocity((p2Vel - (normal * ((gFloat)2.0f * p2Vel.DotProduct(normal)))) * coeffRestitution);
	}
}
#endif

void ParticleContact::ResolveInterpenetration()
{
	gFloat totalInverseMass = p1->GetInverseMass();
	if(p2 != nullptr)
		totalInverseMass += p2->GetInverseMass();

	// Both particles infinitly massive? Nothing to do here
	if(totalInverseMass <= 0) return;

	// Apply change in position to each particle proportional to its inverseMass
	Vector movePerIMass = normal * (penetrationDepth / totalInverseMass);

	// Calculate and apply movement
	p1Movement = movePerIMass * p1->GetInverseMass();
	p1->ForceSetPosition(p1->GetPosition() + p1Movement);

	if(p2 != nullptr)
	{
		p2Movement = movePerIMass * -p2->GetInverseMass();
		p2->ForceSetPosition(p2->GetPosition() + p2Movement);
	}
}