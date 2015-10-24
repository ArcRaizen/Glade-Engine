#include "ParticleSystemPlaneContactGenerator.h"

using namespace Glade;

ParticleSystemPlaneContactGenerator::ParticleSystemPlaneContactGenerator(std::vector<Particle*>& system, Vector normal, gFloat d_, gFloat res, gFloat rad) :
							planeNormal(normal.Normalized()), d(d_), coeffRestitution(res), radius(rad), ParticleSystemContactGenerator(system)
{
}


ParticleSystemPlaneContactGenerator::~ParticleSystemPlaneContactGenerator()
{
}

unsigned int ParticleSystemPlaneContactGenerator::GenerateContact(ParticleContact* contacts, unsigned int limit) const
{
	unsigned int count = 0;	// number of contacts generated
	gFloat t;
	for(auto p = particleSystem.begin(); p != particleSystem.end(); p++)
	{
		// Signed distance from Particle to Plane in direction of planeNormal
		t = planeNormal.DotProduct((*p)->GetPosition()) - d;

		// Distance from Particle to Plane is larger than the Particle's radius, then no Contact
		if(t - radius > 0) continue;

		// Genreate contact
		contacts->SetNewContact(*p, nullptr, coeffRestitution, planeNormal, radius-t);

		// Move to next contact in array
		contacts++;
		count++;

		if(count > limit)
			return count;
	}

	return count;
}