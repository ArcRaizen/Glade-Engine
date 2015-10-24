#pragma once
#ifndef GLADE_PARTICLE_SYSTEM_PLANE_CONTACT_GENERATOR_H
#define GLADE_PARTICLE_SYSTEM_PLANE_CONTACT_GENERATOR_H

#ifndef GLADE_PARTICLE_SYSTEM_CONTACT_GENERATOR_H
#include "ParticleSystemContactGenerator.h"
#endif

namespace Glade {
/*
	Generates Contact for each Particle in the Particle System that passes into/through/behind a define Plane,
	preventing the Particles from going beyond this plane.
*/
class ParticleSystemPlaneContactGenerator : public ParticleSystemContactGenerator
{
public:
	ParticleSystemPlaneContactGenerator(std::vector<Particle*>& system, Vector normal, gFloat d_, gFloat res, gFloat rad=0);
	~ParticleSystemPlaneContactGenerator();

	virtual unsigned int GenerateContact(ParticleContact* contacts, unsigned int limit) const;

protected:
	Vector planeNormal;			// Plane normal
	gFloat d;					// Distance betweeen plane and origin
		// (planeNormal.DotProduct(P) = d) for any point P on the plane
	gFloat radius;				// Radius of Particles in the system
	gFloat coeffRestitution;
};
}	// namespace
#endif	// GLADE_PARTICLE_SYSTEM_PLANE_CONTACT_GENERATOR_H

