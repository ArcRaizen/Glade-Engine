#include "Particle.h"
#include "ParticleContact.h"

using namespace Glade;

unsigned int Particle::PARTICLE_MESH_STACKS = 10;
unsigned int Particle::PARTICLE_MESH_SLICES = 10;
D3DXVECTOR4 Particle::PARTICLE_MESH_COLOR = D3DXVECTOR4(0,0,1,1);

Particle::Particle() : Object(Vector(), Vector(), 0.01, 1, false), motion(30.0f), motionBias(Pow(0.5, PHYSICS_TIMESTEP))
{
	radius = 1;
	shaderResource = GraphicsLocator::GetGraphics()->CreateBuffers(
		GeometryGenerator::CreateSphere(radius, 10, 10, PARTICLE_MESH_COLOR), "../GladeEngine/box.png");
	shaderResource->world = &transformationMatrix;

	transformationMatrix(3,0) = position.x;
	transformationMatrix(3,1) = position.y;
	transformationMatrix(3,2) = position.z;
	CalcBoundingBox();
}

Particle::Particle(Vector pos, Vector vel, Vector accel, gFloat damp, gFloat rad, gFloat iMass, bool ug, Vector grav) : Object(pos, accel, iMass, damp, ug, grav), motion(30.0f), motionBias(Pow(0.5, PHYSICS_TIMESTEP))
#ifndef VERLET
																										, velocity(vel), velocityModified(false)
#else
																										, prevPosition(pos - vel)
#endif
{
	radius = rad;
	shaderResource = GraphicsLocator::GetGraphics()->CreateBuffers(
		GeometryGenerator::CreateSphere(radius, 10, 10, PARTICLE_MESH_COLOR), "../GladeEngine/box.png");
	shaderResource->world = &transformationMatrix;

	transformationMatrix(3,0) = position.x;
	transformationMatrix(3,1) = position.y;
	transformationMatrix(3,2) = position.z;
	CalcBoundingBox();
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
	velocity *= Pow(1-linearDamping, PHYSICS_TIMESTEP);

	acceleration = force * inverseMass;
	Vector nextPos = position + velocity + (acceleration * HALF_PHYSICS_TIMESTEP_SQR);
	prevPosition = position;
	position = nextPos;
#endif

	// Update transformation matrix
	transformationMatrix(3,0) = position.x;
	transformationMatrix(3,1) = position.y;
	transformationMatrix(3,2) = position.z;

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
		if(motion < particleSleepEpsilon)
			SetAwake(false);
		else if(motion > 10 * particleSleepEpsilon)
			motion = 10 * particleSleepEpsilon;
	}
	return true;
}

void Particle::Render() { GraphicsLocator::GetGraphics()->Render(shaderResource, 0); }

void Particle::CalcBoundingBox()
{
	// Particle hasn't moved since last calculation, no need to do it again
	if(!recalcAABB) return;

	boundingBox.minimum.x = position.x - radius;
	boundingBox.minimum.y = position.y - radius;
	boundingBox.minimum.z = position.z - radius;
	boundingBox.maximum.x = position.x + radius;
	boundingBox.maximum.y = position.y + radius;
	boundingBox.maximum.z = position.z + radius;
	recalcAABB = false;
}

void Particle::SetAwake(bool awake/*=true*/)
{
	if(awake)
	{
		isAwake = true;
		motion = particleSleepEpsilon * 2.0f;	// Add motion to prevent it immediately falling asleep
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

Vector Particle::GetVelocity() const
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

Matrix Particle::GetInverseInertiaTensorWorld() const { return inverseInertiaTensor; }
void Particle::GetInverseInertiaTensorWorld(Matrix* m) const { *m = inverseInertiaTensor; }

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