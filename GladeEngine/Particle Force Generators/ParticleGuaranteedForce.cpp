#include "ParticleGuaranteedForce.h"

using namespace Glade;

ParticleGuaranteedForce::ParticleGuaranteedForce(const Vector& f) : force(f), ParticleForceGenerator()
{
}


ParticleGuaranteedForce::~ParticleGuaranteedForce()
{
}

void ParticleGuaranteedForce::GeneratorForce(Particle* particle)
{
	// Cannot apply force to particle of infinite mass
	if(particle->GetInverseMass() == 0) return;

	particle->ApplyForce(force);
}