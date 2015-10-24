#include "Particle.h"
#include "ParticleContact.h"

using namespace Glade;

Particle::Particle() :Object(Vector(), Vector(), 1, false), damping(0.01)
{
	radius = 1;
}

Particle::Particle(Vector pos, Vector vel, Vector accel, gFloat damp, gFloat iMass, gFloat rad, bool ug, Vector grav) : Object(pos, accel, iMass, ug, grav), damping(damp)
#ifndef VERLET
																										, velocity(vel), velocityModified(false)
#else
																										, prevPosition(pos - vel)
#endif
{
	radius = rad;
}


Particle::~Particle()
{
}


// Integrate particle forawrd in time using Velocity Verlet integration
bool Particle::Update()
{
	// Don't integrate things with infinite masses (they can't move)
	if(inverseMass == 0 || !isAwake) return false;

#ifndef VERLET
	// Velocity Verlet Integration
	lastAcceleration = acceleration;
	position += (velocity * PHYSICS_TIMESTEP) + (lastAcceleration * HALF_PHYSICS_TIMESTEP_SQR);
	Vector newAccel = force * inverseMass;
	Vector avgAccel = ((lastAcceleration + newAccel) / (gFloat)2);
	velocity += avgAccel * PHYSICS_TIMESTEP;
	acceleration = newAccel;

	// Impose drag
	velocity *= Pow(1-damping, PHYSICS_TIMESTEP);
	velocityModified = true;
#else
	velocity = position - prevPosition;

	// Impose drag
	velocity *= Pow(1-damping, PHYSICS_TIMESTEP);

	acceleration = force * inverseMass;
	Vector nextPos = position + velocity + (acceleration * HALF_PHYSICS_TIMESTEP_SQR);
	prevPosition = position;
	position = nextPos;
#endif

	// Clear forces for next frame
	force.Zero();
	if(useGravity)
		ApplyForce(gravity);

	// Particle is awake, which means it's moving, which means its boundingBox needs to be updated
	recalcAABB = true;

	// Check motion/sleep
	if(canSleep)
	{
		motion = (motionBias * motion) + ((1 - motionBias) * velocity.DotProduct(velocity));
		if(motion < sleepEPSILON)
			SetAwake(false);
		else if(motion > 10 * sleepEPSILON)
			motion = 10 * sleepEPSILON;
	}
	return true;
}

void Particle::CalcBoundingBox()
{
	// Particle hasn't moved since last calculation, no need to do it again
	if(!recalcAABB) return;

	boundingBox.min.x = position.x - radius;
	boundingBox.min.y = position.y - radius;
	boundingBox.min.z = position.z - radius;
	boundingBox.max.x = position.x + radius;
	boundingBox.max.y = position.y + radius;
	boundingBox.max.z = position.z + radius;
	recalcAABB = false;
}

void Particle::SetAwake(bool awake/*=true*/)
{
	if(awake)
	{
		isAwake = true;
		motion = sleepEPSILON * 2.0f;	// Add motion to prevent it immediately falling asleep
	}
	else
	{
		isAwake = false;
#ifndef VERLET
		velocity.Zero();
#else
		prevPosition = position;
#endif
	}
}

void Particle::RegisterForceGenerator(int id)
{
	for(unsigned int i = 0; i < generatorIDs.size(); ++i)
	{
		if(generatorIDs[i] == id) return;
	}

	generatorIDs.push_back(id);
}

void Particle::UnregisterForceGenerator(int id)
{
	for(unsigned int i = 0; i < generatorIDs.size(); ++i)
	{
		if(generatorIDs[i] == id)
		{
			generatorIDs.erase(generatorIDs.begin() + i);
			return;
		}
	}
}

std::vector<int> Particle::GetRegisteredForceGenerators()
{
	return generatorIDs;
}


void Particle::AllowSetVelocity() {	properties |= OVERRIDE_VELOCITY; }
void Particle::DisallowSetVelocity() { properties &= ~OVERRIDE_VELOCITY; }
bool Particle::CheckAllowSetVelocity() { return properties & OVERRIDE_VELOCITY; }

void Particle::GetVelocity(Vector& v) const { v = velocity; }
void Particle::GetVelocity(Vector* v) const { *v = velocity; }

Vector Particle::GetVelocity()
{
#ifndef VERLET
	velocityModified = false;
#endif
	return velocity;
}

void Particle::SetVelocity(const Vector& v)
{
	if(!(properties & OVERRIDE_VELOCITY)) return;
#ifndef VERLET
	velocity = v;
#else
	prevPosition = position - v;
#endif
}

void Particle::SetVelocity(const gFloat x, const gFloat y, const gFloat z)
{
	if(!(properties & OVERRIDE_VELOCITY)) return;
#ifndef VERLET
	velocity.x = x;
	velocity.y = y;
	velocity.z = z;
#else
	prevPosition.x = position.x - x;
	prevPosition.y = position.y - y;
	prevPosition.z = position.z - z;
#endif
}

void Particle::ForceSetPosition(const Vector& p) { position = p; }
void Particle::ForceSetVelocity(const Vector& v)
{
#ifndef VERLET
	velocity = v;
	velocityModified = true;
#else
	prevPosition = position - v;
#endif
}
void Particle::ForceSetAcceleration(const Vector& a) { acceleration = a; }