#include "ParticleBuoyancyForceGenerator.h"

using namespace Glade;

ParticleBuyoancyForceGenerator::ParticleBuyoancyForceGenerator(const Vector pointOnSurface, const Vector dir, gFloat depth, gFloat vol, gFloat density,	gFloat rad, gFloat maxDist) : 
									ParticleDirectionalForceGenerator(pointOnSurface,dir,Vector(), ForceScaling::NONE, rad, false, maxDist), 
									maxDepth(depth), volume(vol), liquidDensity(density)
{
}

ParticleBuyoancyForceGenerator::~ParticleBuyoancyForceGenerator()
{
}

void ParticleBuyoancyForceGenerator::GenerateForce(Particle* particle)
{
	Vector partPos = particle->GetPosition();

	// Signed distance of particle from plane defined by pointOnSurface/origin and direction of force
	gFloat t = direction.DotProduct(partPos - origin);
	
	// No buoyancy force if particle is above the water
	if(t > maxDepth) return;

	// Pecent of particle submerged
	gFloat penetrationRatio;

	if(t < -maxDepth)	// Completely submerged
		penetrationRatio = 1;	
	else				// Partially submerged
		penetrationRatio = (t - maxDepth) / (2 * maxDepth);

	// Apply buoyancy force (scales directly with percent of particle submerged)
	particle->ApplyForce(direction * (liquidDensity*volume) * penetrationRatio);
}