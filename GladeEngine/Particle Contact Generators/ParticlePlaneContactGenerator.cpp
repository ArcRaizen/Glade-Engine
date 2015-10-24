#include "ParticlePlaneContactGenerator.h"

using namespace Glade;

ParticlePlaneContactGenerator::ParticlePlaneContactGenerator(Particle* p1_, Vector normal, gFloat d_, gFloat res, gFloat rad) : 
															p1(p1_), planeNormal(normal.Normalized()), d(d_), coeffRestitution(res), radius(rad)
{
}


ParticlePlaneContactGenerator::~ParticlePlaneContactGenerator()
{
}

unsigned int ParticlePlaneContactGenerator::GenerateContact(ParticleContact* contacts, unsigned int limit) const
{
	// Signed distance from Particle to Plane in direction of planeNormal
	gFloat t = planeNormal.DotProduct(p1->GetPosition()) - d;

	// Distance from Particle to Plane is large than the Particle's radius, then no Contact
	if(t - radius > 0) return 0;

	// Generate Contact
	contacts->SetNewContact(p1, nullptr, coeffRestitution, planeNormal, radius-t);
	return 1;
}