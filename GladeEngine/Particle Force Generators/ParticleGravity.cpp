#include "ParticleGravity.h"

using namespace Glade;

ParticleGravity::ParticleGravity(const Vector grav) : ParticleGuaranteedForce(grav)
{
}


ParticleGravity::~ParticleGravity()
{
}

// Apply force scaled to particle's mass
void ParticleGravity::GenerateForce(Particle* particle)
{
	// F = m*a		so if we apply a force 'F' that is equivalent to some force 'f' times 'm', we get
	// f*m = m*a	'm' cancels on both sides, therefore we get f = a
	// Meaning the 'f' we apply direcetly affects the acceleration regardless of mass, so all particles
	// accerlerate at the rate of 'f'
	particle->ApplyForce(force * particle->GetMass());	
}