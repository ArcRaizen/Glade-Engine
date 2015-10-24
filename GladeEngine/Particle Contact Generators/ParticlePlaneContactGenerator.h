#pragma once
#ifndef GLADE_PARTICLE_PLANE_CONTACT_GENERATOR_H
#define GLADE_PARTICLE_PLANE_CONTACT_GENERATOR_H

#ifndef GLADE_PARTICLE_CONTACT_GENERATOR_H
#include "ParticleContactGenerator.h"
#endif

namespace Glade {
/*
	Generates Contact if a Particle passes into/through/behind a defined Plane,
	preventing the Particle from going beyond this plane.
*/
class ParticlePlaneContactGenerator : public ParticleContactGenerator
{
public:
	ParticlePlaneContactGenerator(Particle* p1_, Vector normal, gFloat d_, gFloat res, gFloat rad=0);
	~ParticlePlaneContactGenerator();

	virtual unsigned int GenerateContact(ParticleContact* contacts, unsigned int limit) const;

protected:
	Particle* p1;

	Vector	planeNormal;	// Plane normal
	gFloat	d;				// Distance between plane and origin
		// (planeNormal.DotProduct(P) = d) for any point P on the plane
	gFloat radius;			// Radius of p1

	gFloat coeffRestitution;
};
}	// namespace
#endif	// GLADE_PARTICLE_PLANE_CONTACT_GENERATOR_H
