#include "ParticleAttractorForce.h"

using namespace Glade;

ParticleAttractorForce::ParticleAttractorForce(const Vector grav, const Vector attract) : attractionPoint(attract), ParticleGravity(grav)
{
}


ParticleAttractorForce::~ParticleAttractorForce()
{
}

void ParticleAttractorForce::GenerateForce(Particle* particle)
{
	particle->ApplyForce((attractionPoint - particle->GetPosition()).Normalized() * force.Magnitude() * particle->GetMass());
}