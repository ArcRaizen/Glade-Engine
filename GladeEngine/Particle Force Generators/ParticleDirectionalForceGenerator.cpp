#include "ParticleDirectionalForceGenerator.h"

using namespace Glade;

ParticleDirectionalForceGenerator::ParticleDirectionalForceGenerator(const Vector o, const Vector dir, const Vector force, ForceScaling s, gFloat rad, bool push, gFloat maxDist) : 
	origin(o), direction(dir.Normalized()), forceAtOrigin(force), scaling(s), radius(rad), pushFromOrigin(push), maxDistance(maxDist)
{
}


ParticleDirectionalForceGenerator::~ParticleDirectionalForceGenerator()
{
}

void ParticleDirectionalForceGenerator::GenerateForce(Particle* particle)
{
	if(particle->GetInverseMass() == 0) return;

	Vector partPos = particle->GetPosition();

	// Signed distance of particle from plane defined by origin and direction of force
	gFloat t = direction.DotProduct(partPos - origin);

	// No force if particle is behind origin or too far from it
	if(pushFromOrigin && (t < 0 || t > maxDistance)) return;
	if(t > 0 || t < -maxDistance) return;

	// No force if particle is too far away from origin
	if(partPos - (direction * t) > radius) return;

	switch(scaling)
	{
		case ForceScaling::NONE:
			particle->ApplyForce(forceAtOrigin);
			return;
		case ForceScaling::LINEAR:
			particle->ApplyForce(forceAtOrigin * (1 - (t/maxDistance)));
			return;
		case ForceScaling::SQUARED:
			particle->ApplyForce(forceAtOrigin * (1 - Pow(t/maxDistance, 2)));
			return;
		case ForceScaling::LOG:
			return;
	}
}