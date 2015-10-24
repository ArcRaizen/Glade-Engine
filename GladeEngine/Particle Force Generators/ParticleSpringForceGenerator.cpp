#include "ParticleSpringForceGenerator.h"

using namespace Glade;

ParticleSpringForceGenerator::ParticleSpringForceGenerator(const Vector& v, gFloat k, gFloat l, bool extended, bool compressed) : 
	anchor(v), springConstant(l), restLength(l), exertWhileExtended(extended), exertWhileCompressed(compressed)
{
}


ParticleSpringForceGenerator::~ParticleSpringForceGenerator()
{
}

void ParticleSpringForceGenerator::GenerateForce(Particle* particle)
{
	// Calc vector of the spring
	Vector spring = particle->GetPosition() - anchor;

	// Do not exert force if that force is turned off on this spring
	gFloat springLength = spring.Magnitude();
	if(!exertWhileExtended && springLength > restLength) return;
	if(!exertWhileCompressed && springLength < restLength) return;

	// Calc magnitude of the force (F = k * (curLength - restLength))
	gFloat mag = (springLength - restLength) * springConstant;

	// Apply force
	particle->ApplyForce(spring.Normalized() * mag);
}

void ParticleSpringForceGenerator::SetAnchor(const Vector& v)
{
	anchor = v;
}

/*
 // Calculate the vector of the spring.
    Vector3 force;
    particle->getPosition(&force);
    force -= other->getPosition();

    // Calculate the magnitude of the force.
    real magnitude = force.magnitude();
    magnitude = real_abs(magnitude - restLength);
    magnitude *= springConstant;

    // Calculate the final force and apply it.
    force.normalize();
    force *= -magnitude;
    particle->addForce(force);
*/