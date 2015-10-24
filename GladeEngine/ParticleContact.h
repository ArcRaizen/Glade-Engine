#pragma once
#ifndef GLADE_PARTICLE_CONTACT_H
#define GLADE_PARTICLE_CONTACT_H

#ifndef GLADE_PARTICLE_H
#include "Particle.h"
#endif

namespace Glade {
// Forward declare ParticleContactResolver class now
class ParticleContactResolver;

class ParticleContact
{
public:
	ParticleContact();
	ParticleContact(Particle* p1_, Particle* p2_, gFloat res, Vector n, gFloat pen);
	~ParticleContact();

	void SetNewContact(Particle* p1_, Particle* p2_, gFloat res, Vector n, gFloat pen);

	friend class ParticleContactResolver;
	bool operator<(const ParticleContact& other) { return rel < other.rel; }

protected:
	// Perform calculations and actions necesary to resolve this contact
	void	Resolve();

	// Update the awake state of Particles that are involved in this Contact.
	// A Particle will be made awake if it is in contact with a Particle that is awake.
	void	MatchAwakeState();

	// Calculate the Relative Velocity of the particles as they collided
	gFloat	CalcRelativeVelocity();

private:
#ifndef VERLET
	// Impulse Calculation and Resolution
	void	ResolveImpulse();
#else
	void	ResolveImpulseVerlet();
#endif

	// Interpenetration Calculation and Resolution
	void	ResolveInterpenetration();

	Particle*	p1;					// First particle in the contact
	Particle*	p2;					// Second particle in the contact
	gFloat		coeffRestitution;	// Coefficient of Restitution for the contact
	Vector		normal;				// Contact normal in world coordinates
	gFloat		penetrationDepth;	// Depth of penetration at point of contact
	Vector		p1Movement;			// Particle 1's movement when resolving interpenetration
	Vector		p2Movement;			// Particle 2's movement when resolving interpenetration

//#ifndef VERLET
	Vector		relativeV;			// Relative Velocity of Particle(s) in the contact
	gFloat		rel;				// Magnitude of the Relative Velocity of the Particle(s) in the Contact
										// in the direction of the Contact Normal
//#endif
};
}	// namespace
#endif	// GLADE_PARTICLE_CONTACT_H