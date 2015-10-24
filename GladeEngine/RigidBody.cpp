#include "RigidBody.h"

using namespace Glade;

RigidBody::RigidBody()
{
}

RigidBody::RigidBody(Vector pos, Quaternion orient, Vector vel, Vector accel, Vector angVel, Vector angAccel, gFloat lDamp, gFloat aDamp, gFloat iMass, bool ug, Vector grav) : 
			Object(pos, accel, iMass, ug, grav), orientation(orient), velocity(vel), angularVelocity(angVel), angularAcceleration(angAccel), linearDamping(lDamp), angularDamping(aDamp)
{
	CalcDerivedData();
}

RigidBody::~RigidBody()
{
}

bool RigidBody::Update()
{
	// Don't integrate things with infinite masses (they can't move)
	if(inverseMass == 0 || !isAwake) return false;

	// Velocity Verlet Integration
	lastAcceleration = acceleration;
	position += (velocity * PHYSICS_TIMESTEP) + (lastAcceleration * HALF_PHYSICS_TIMESTEP_SQR);
	Vector newAccel = force * inverseMass;
	Vector avgAccel = ((lastAcceleration + newAccel) / (gFloat)2.0);
	velocity += avgAccel * PHYSICS_TIMESTEP;
	acceleration = newAccel;


	// q(t) - orientation quaternion | w - angular velocity
	// 
	// dq/dt = (1/2) * w * q(t)
	// q(t+Δt) = q(t) + (dq/dt * Δt) = q(t) + ((1/2) * Δt * w * q(t))
	// https://fgiesen.wordpress.com/2012/08/24/quaternion-differentiation/#comment-3913
	lastAngularAcceleration = angularAcceleration;
	orientation += ((Quaternion(angularVelocity.x, angularVelocity.y, angularVelocity.z, 0.0f) * orientation) * (HALF_PHYSICS_TIMESTEP));
	Vector newAngAccel = torque * inverseInertiaTensorWorld;
	Vector avgAngAccel = ((lastAngularAcceleration + newAngAccel) / (gFloat)2.0);
	angularVelocity += avgAngAccel * PHYSICS_TIMESTEP;
	angularAcceleration = newAngAccel;

	// Impose drag
	velocity *= Pow(linearDamping, PHYSICS_TIMESTEP);
	angularVelocity *= Pow(angularDamping, PHYSICS_TIMESTEP);

	// Normalize orientation and update transformation matrix and world inertia tensor
	CalcDerivedData();

	// RigidBody is awake, which means it's moving, which means its boundingBox needs to be updated
	CalcBoundingBox();

	// Clear forces/torques for next frame
	force.Zero();
	torque.Zero();
	if(useGravity)
		ApplyForce(gravity);

	// Check motion/sleep
	if(canSleep)
	{
		motion = (motionBias * motion) + ((1 - motionBias) * velocity.DotProduct(velocity) * angularVelocity.DotProduct(angularVelocity));
		if(motion < sleepEPSILON)
			SetAwake(false);
		else if(motion > 10 * sleepEPSILON)
			motion = 10 * sleepEPSILON;
	}

	return true;
}

void RigidBody::CalcDerivedData()
{
	// Safety - Make sure orientation quaternion is normalized
	orientation.NormalizeInPlace();

	// Calculate transformation matrix from its current state
	transformationMatrix.ComposeTransformationMatrix(&position, &orientation, nullptr);

	// Calculate inverse inertia tensor in world space
//	inverseInertiaTensorWorld = ((transformationMatrix.Inverse3() * inverseInertiaTensor) * transformationMatrix.GetMatrix3());
	inverseInertiaTensorWorld = transformationMatrix.MultiplyInverse3(inverseInertiaTensor).Multiply3(transformationMatrix);

}

void RigidBody::CalcBoundingBox()
{
	// Calculate Bounding Box from colliders
	if(colliders.size() > 0)
	{
		colliders[0]->CalcTransformAndDerivedGeometricData(transformationMatrix);
		boundingBox = colliders[0]->GetBounds();
		AABB colliderBounds;
		for(unsigned int i = 1; i < colliders.size(); ++i)
		{
			colliders[i]->CalcTransformAndDerivedGeometricData(transformationMatrix);
			colliderBounds = colliders[i]->GetBounds();
			if(colliderBounds.max.x > boundingBox.max.x)	boundingBox.max.x = colliderBounds.max.x;
			if(colliderBounds.max.y > boundingBox.max.y)	boundingBox.max.x = colliderBounds.max.y;
			if(colliderBounds.max.z > boundingBox.max.z)	boundingBox.max.x = colliderBounds.max.z;
			if(colliderBounds.min.x < boundingBox.min.x)	boundingBox.min.x = colliderBounds.min.x;
			if(colliderBounds.min.y < boundingBox.min.y)	boundingBox.min.x = colliderBounds.min.y;
			if(colliderBounds.min.z < boundingBox.min.z)	boundingBox.min.x = colliderBounds.min.z;
		}
	}
	else
	{
		/*
		Vector min = vertices[0], max = vertices[0];
		for(unsigned int i = 1; i < numVertices; ++i)
		{
			if(vertices[i].x < min.x) min.x = vertices[i].x;
			else if(vertices[i].x > max.x) max.x = vertices[i].x;
			if(vertices[i].y < min.y) min.y = vertices[i].y;
			else if(vertices[i].y > max.y) max.y = vertices[i].y;
			if(vertices[i].z < min.x) min.z = vertices[i].z;
			else if(vertices[i].z > max.z) max.z = vertices[i].x;
		}

		boundingBox.min = min;
		boundingBox.max = max;
		*/
	}

	// Calculate maximum radius of AABB
	radius = (boundingBox.max - boundingBox.min).Magnitude() / (gFloat)2.0;

	recalcAABB = false;
}

void RigidBody::ApplyForceAtPoint(const Vector& f, const Vector& p)
{
	force += f;
	torque += (p - position).CrossProduct(f);
	isAwake = true;
}

void RigidBody::ApplyForceAtLocalPoint(const Vector& f, const Vector& p)
{
	force += f;
	torque += (GetPointInWorldSpace(p) - position).CrossProduct(f);
	isAwake = true;
}

void RigidBody::SetAwake(bool awake/*=true*/)
{
	if(awake)
	{
		isAwake = true;
		motion = sleepEPSILON * 2.0f;	// Add motion to prevent it immediately falling asleep
	}
	else
	{
		isAwake = false;
		velocity.Zero();
		angularVelocity.Zero();
	}
}

void RigidBody::RegisterForceGenerator(int id)
{
	for(unsigned int i = 0; i < generatorIDs.size(); ++i)
	{
		if(generatorIDs[i] == id) return;
	}

	generatorIDs.push_back(id);
}

void RigidBody::UnregisterForceGenerator(int id)
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

std::vector<int> RigidBody::GetRegisteredForceGenerators()
{
	return generatorIDs;
}


void RigidBody::AllowSetVelocity() { properties |= OVERRIDE_VELOCITY; }
void RigidBody::DisallowSetVelocity() { properties &= ~OVERRIDE_VELOCITY; }
bool RigidBody::CheckAllowSetVelocity() { return properties & OVERRIDE_VELOCITY; }


Vector RigidBody::GetVelocity()
{
	velocityModified = false;
	return velocity;
}
void RigidBody::SetVelocity(const Vector& v)
{
	if(!(properties & OVERRIDE_VELOCITY)) return;
	velocity = v;
}
void RigidBody::SetVelocity(const gFloat x, const gFloat y, const gFloat z)
{
	if(!(properties & OVERRIDE_VELOCITY)) return;
	velocity.x = x;
	velocity.y = y;
	velocity.z = z;
}
Vector RigidBody::GetAngularVelocity() 
{ 
	angularVelocityModified = false;
	return angularVelocity; 
}
void RigidBody::SetAngularVelocity(const Vector& v)
{
	if(!(properties & OVERRIDE_ANGULAR_VELOCITY)) return;
	angularVelocity = v;
}
void RigidBody::SetAngularVelocity(const gFloat x, const gFloat y, const gFloat z)
{
	if(!(properties & OVERRIDE_VELOCITY)) return;
	angularVelocity.x = x;
	angularVelocity.y = y;
	angularVelocity.z = z;
}

void RigidBody::SetInverseInertiaTensor(const Matrix& m) { inverseInertiaTensor = m; CalcDerivedData(); }

Matrix RigidBody::GetTransformMatrix() { return transformationMatrix; }
Matrix RigidBody::GetInverseInertiaTensorWorld() { return inverseInertiaTensorWorld; }

Vector RigidBody::GetLastFrameAcceleration() const { return lastAcceleration; }
void RigidBody::ForceSetPosition(const Vector& p) { position = p; }
void RigidBody::ForceSetOrientation(const Quaternion& o) { orientation = o; }
void RigidBody::ForceSetVelocity(const Vector& v) { velocityModified = true; velocity = v; }
void RigidBody::ForceSetAngularVelocity(const Vector& v) { angularVelocityModified = true; angularVelocity = v; }
void RigidBody::ForceSetAcceleration(const Vector& a) { acceleration = a; }
void RigidBody::ForceSetAngularAcceleration(const Vector& a) { angularAcceleration = a; }
void RigidBody::ForceAddPosition(const Vector& p) { position += p; }
void RigidBody::ForceAddOrientation(const Vector& o) { orientation += ((Quaternion(o.x,o.y,o.z,0.0f) * orientation) * 0.5f); }
void RigidBody::ForceAddVelocity(const Vector& v) { 
	velocityModified = true; 
	velocity += v; }
void RigidBody::ForceAddAngularVelocity(const Vector& v) { angularVelocityModified = true; angularVelocity += v; }
void RigidBody::ForceAddAcceleration(const Vector& a) { acceleration += a; }
void RigidBody::ForceAddAngularAcceleration(const Vector& a) { angularAcceleration += a; }