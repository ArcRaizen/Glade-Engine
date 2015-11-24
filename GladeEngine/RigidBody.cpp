#include "RigidBody.h"

using namespace Glade;

RigidBody::RigidBody()
{
}

RigidBody::RigidBody(Vector pos, Quaternion orient, Vector vel, Vector accel, Vector angVel, Vector angAccel, gFloat lDamp, gFloat aDamp, gFloat iMass, bool ug, Vector grav) : 
			Object(pos, accel, iMass, ug, grav), orientation(orient), velocity(vel), angularVelocity(angVel), angularAcceleration(angAccel), linearDamping(lDamp), angularDamping(aDamp)
#ifdef SLEEP_TEST_ENERGY
			, motion(30.0f), motionBias(Pow(0.5, PHYSICS_TIMESTEP))
#endif
{
	CalcDerivedData();

#ifdef SLEEP_TEST_BOX
	InitializeSleepBoxes();
#endif
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
//	orientation += Quaternion(angularVelocity.x*HALF_PHYSICS_TIMESTEP, angularVelocity.y*HALF_PHYSICS_TIMESTEP, angularVelocity.z*HALF_PHYSICS_TIMESTEP, 0.0f) * orientation;
//	orientation += orientation * Quaternion(angularVelocity.x*HALF_PHYSICS_TIMESTEP, angularVelocity.y*HALF_PHYSICS_TIMESTEP, angularVelocity.z*HALF_PHYSICS_TIMESTEP, 0.0f);
	orientation.AddRotation(angularVelocity, PHYSICS_TIMESTEP);
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
#ifdef SLEEP_TEST_ENERGY
		motion = (motionBias * motion) + ((1 - motionBias) * velocity.DotProduct(velocity) * angularVelocity.DotProduct(angularVelocity));
		if(motion < sleepEpsilon)
			SetAwake(false);
		else if(motion > 10 * sleepEpsilon)
			motion = 10 * sleepEpsilon;
#elif defined SLEEP_TEST_BOX
		// Update sleep boxes after frame of motion
		Vector sleepBoxPosistions[3] = {position, position + Vector(transformationMatrix(0,0), transformationMatrix(0,1), transformationMatrix(0,2)),
										position + Vector(transformationMatrix(1,0), transformationMatrix(1,1), transformationMatrix(1,2))};
		for(unsigned int i = 0; i < 3; ++i)
		{
			if(sleepBoxPosistions[i].x < sleepBoxes[i].min.x) sleepBoxes[i].min.x = sleepBoxPosistions[i].x;
			else if(sleepBoxPosistions[i].x > sleepBoxes[i].max.x) sleepBoxes[i].max.x = sleepBoxPosistions[i].x;
			if(sleepBoxPosistions[i].y < sleepBoxes[i].min.y) sleepBoxes[i].min.y = sleepBoxPosistions[i].y;
			else if(sleepBoxPosistions[i].y > sleepBoxes[i].max.y) sleepBoxes[i].max.y = sleepBoxPosistions[i].y;
			if(sleepBoxPosistions[i].z < sleepBoxes[i].min.z) sleepBoxes[i].min.z = sleepBoxPosistions[i].z;
			else if(sleepBoxPosistions[i].z > sleepBoxes[i].max.z) sleepBoxes[i].max.z = sleepBoxPosistions[i].z;
		}

		// Get max dimensions of the sleep boxes
		gFloat maxLinearDimension = MAX(MAX(sleepBoxes[0].Width(), sleepBoxes[0].Height()), sleepBoxes[0].Depth());
		gFloat maxAngularDimension =  MAX(MAX(MAX(sleepBoxes[1].Width(), sleepBoxes[1].Height()), sleepBoxes[1].Depth()), 
									MAX(MAX(sleepBoxes[2].Width(), sleepBoxes[2].Height()), sleepBoxes[2].Depth()));

		// If any sleep box is larger than the maximum threshold -> The sleep test has failed: reset them, the object isn't going to sleep yet
		if(maxLinearDimension > sleepLinearEpsilon || maxAngularDimension > sleepAngularEpsilon)
			InitializeSleepBoxes();
		// If its been a sufficient time since the sleep test failed, the object has sufficiently ceased moving - Put it to sleep
		else if(++sleepSteps >= sleepStepThreshold)
			SetAwake(false);
#endif
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
//	inverseInertiaTensorWorld = transformationMatrix.Multiply3(inverseInertiaTensor).MultiplyInverse3(transformationMatrix);
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

void RigidBody::InitializeSleepBoxes()
{
	sleepSteps = 0;
	sleepBoxes[0].min = sleepBoxes[0].max = position;
	sleepBoxes[1].min = sleepBoxes[1].max = position + Vector(transformationMatrix(0,0), transformationMatrix(0,1), transformationMatrix(0,2));
	sleepBoxes[2].min = sleepBoxes[2].max = position + Vector(transformationMatrix(1,0), transformationMatrix(1,1), transformationMatrix(1,2));
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
	// Infinitely massed Objects are never awake to prevent them waking up other Objects
	if(inverseMass == (gFloat)0.0f) 
		return;

	if(awake)
	{
		isAwake = true;
#ifdef SLEEP_TEST_ENERGY
		motion = sleepEpsilon * 2.0f;	// Add motion to prevent it immediately falling asleep
#elif defined SLEEP_TEST_BOX
		InitializeSleepBoxes();
#endif
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


Vector RigidBody::GetVelocity() { return velocity; }
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
Vector RigidBody::GetAngularVelocity() { return angularVelocity; }
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
void RigidBody::ForceSetVelocity(const Vector& v) { velocity = v; }
void RigidBody::ForceSetAngularVelocity(const Vector& v) { angularVelocity = v; }
void RigidBody::ForceSetAcceleration(const Vector& a) { acceleration = a; }
void RigidBody::ForceSetAngularAcceleration(const Vector& a) { angularAcceleration = a; }
void RigidBody::ForceAddPosition(const Vector& p) { position += p; }
void RigidBody::ForceAddOrientation(const Vector& o) { /*orientation += (orientation * (Quaternion(o.x,o.y,o.z,0.0f) * 0.5f));*/
orientation.AddRotation(o, 1);}
void RigidBody::ForceAddVelocity(const Vector& v) {	velocity += v; }
void RigidBody::ForceAddAngularVelocity(const Vector& v) { angularVelocity += v; }
void RigidBody::ForceAddAcceleration(const Vector& a) { acceleration += a; }
void RigidBody::ForceAddAngularAcceleration(const Vector& a) { angularAcceleration += a; }