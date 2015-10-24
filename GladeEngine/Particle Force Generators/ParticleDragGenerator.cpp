#include "ParticleDragGenerator.h"

using namespace Glade;

ParticleDragGenerator::ParticleDragGenerator(const gFloat k1_, const gFloat k2_) : k1(k1_), k2(k2_)
{
}


ParticleDragGenerator::~ParticleDragGenerator()
{
}

void ParticleDragGenerator::GenerateForce(Particle* particle)
{
	Vector vel = particle->GetVelocity();
	gFloat velMag = vel.Magnitude();

	// Calculate final force and apply it (DragForce = -V * (k1*|V| + k2*(|V|^2))
	particle->ApplyForce(vel.Normalized() * -((k1 * velMag) + (k2 * velMag * velMag)));
}