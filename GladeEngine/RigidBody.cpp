#include "RigidBody.h"

using namespace Glade;

RigidBody::RigidBody()
{
}

RigidBody::RigidBody(Vector pos, Quaternion orient, Vector vel, Vector accel, Vector angVel, Vector angAccel, gFloat lDamp, gFloat aDamp, bool ug, Vector grav) : 
			Object(pos, accel, lDamp, ug, grav), orientation(orient), velocity(vel), angularVelocity(angVel), angularAcceleration(angAccel), angularDamping(aDamp)
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
	centroid += (velocity * PHYSICS_TIMESTEP) + (lastAcceleration * HALF_PHYSICS_TIMESTEP_SQR);
	Vector newAccel = force * inverseMass;
	Vector avgAccel = ((lastAcceleration + newAccel) / gFloat(2.0f));
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

	// Clear forces/torques for next frame
	force.Zero();
	torque.Zero();
	if(useGravity)
		ApplyForce(gravity *
#ifdef TRACK_MASS
		mass
#else
		(gFloat)1.0f / inverseMass;
#endif
		);

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
			if(sleepBoxPosistions[i].x < sleepBoxes[i].minimum.x) sleepBoxes[i].minimum.x = sleepBoxPosistions[i].x;
			else if(sleepBoxPosistions[i].x > sleepBoxes[i].maximum.x) sleepBoxes[i].maximum.x = sleepBoxPosistions[i].x;
			if(sleepBoxPosistions[i].y < sleepBoxes[i].minimum.y) sleepBoxes[i].minimum.y = sleepBoxPosistions[i].y;
			else if(sleepBoxPosistions[i].y > sleepBoxes[i].maximum.y) sleepBoxes[i].maximum.y = sleepBoxPosistions[i].y;
			if(sleepBoxPosistions[i].z < sleepBoxes[i].minimum.z) sleepBoxes[i].minimum.z = sleepBoxPosistions[i].z;
			else if(sleepBoxPosistions[i].z > sleepBoxes[i].maximum.z) sleepBoxes[i].maximum.z = sleepBoxPosistions[i].z;
		}

		// Get max dimensions of the sleep boxes
		gFloat maxLinearDimension = Max(Max(sleepBoxes[0].Width(), sleepBoxes[0].Height()), sleepBoxes[0].Depth());
		gFloat maxAngularDimension = Max(Max(Max(sleepBoxes[1].Width(), sleepBoxes[1].Height()), sleepBoxes[1].Depth()), 
									Max(Max(sleepBoxes[2].Width(), sleepBoxes[2].Height()), sleepBoxes[2].Depth()));

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

void RigidBody::Render() { GraphicsLocator::GetGraphics()->Render(shaderResource, 0); }

void RigidBody::CalcDerivedData()
{
	// Safety - Make sure orientation quaternion is normalized
	orientation.NormalizeInPlace();

	// Calculate transformation matrix from its current state
	transformationMatrix.ComposeTransformationMatrix(&centroid, &orientation, nullptr);

	// Update position now that Centroid has moved
	position = transformationMatrix.Times3(-localCentroid) + centroid;

	// Calculate inverse inertia tensor in world space
	inverseInertiaTensorWorld = (transformationMatrix.Times3(inverseInertiaTensor)).TimesTranspose3(transformationMatrix);
//	inverseInertiaTensorWorld = (transformationMatrix.Transpose3Times(inverseInertiaTensor)).Times3(transformationMatrix);


/*
	Matrix test =  (transformationMatrix.Times3(inverseInertiaTensor)).TimesTranspose3(transformationMatrix);
	Matrix test2 = transformationMatrix.Times3((inverseInertiaTensor).TimesTranspose3(transformationMatrix));
	//										^^^ ASSOCIATIVE PAIRS vvv  
	Matrix test3 = (transformationMatrix.Transpose3Times(inverseInertiaTensor)).Times3(transformationMatrix);
	Matrix test4 = transformationMatrix.Transpose3Times((inverseInertiaTensor).Times3(transformationMatrix));
*/
	// RigidBody is moving was moved by a collision, which means its BoundingBox needs to be updated
	CalcBoundingBox();
	solved = false;
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
			if(colliderBounds.maximum.x > boundingBox.maximum.x)	boundingBox.maximum.x = colliderBounds.maximum.x;
			if(colliderBounds.maximum.y > boundingBox.maximum.y)	boundingBox.maximum.x = colliderBounds.maximum.y;
			if(colliderBounds.maximum.z > boundingBox.maximum.z)	boundingBox.maximum.x = colliderBounds.maximum.z;
			if(colliderBounds.minimum.x < boundingBox.minimum.x)	boundingBox.minimum.x = colliderBounds.minimum.x;
			if(colliderBounds.minimum.y < boundingBox.minimum.y)	boundingBox.minimum.x = colliderBounds.minimum.y;
			if(colliderBounds.minimum.z < boundingBox.minimum.z)	boundingBox.minimum.x = colliderBounds.minimum.z;
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
	radius = (boundingBox.maximum - boundingBox.minimum).Magnitude() / (gFloat)2.0;

	recalcAABB = false;
}

#ifdef SLEEP_TEST_BOX
void RigidBody::InitializeSleepBoxes()
{
	sleepSteps = 0;
	sleepBoxes[0].minimum = sleepBoxes[0].maximum = position;
	sleepBoxes[1].minimum = sleepBoxes[1].maximum = position + Vector(transformationMatrix(0,0), transformationMatrix(0,1), transformationMatrix(0,2));
	sleepBoxes[2].minimum = sleepBoxes[2].maximum = position + Vector(transformationMatrix(1,0), transformationMatrix(1,1), transformationMatrix(1,2));
}
#endif

void RigidBody::AddCollider(Collider* collider)
{
	// Infinite Mass collider needs special attention
	// Overrides all other colliders
	if(collider->GetInverseMass() == gFloat(0.0f))
	{
		for(auto iter = colliders.begin(); iter != colliders.end(); ++iter)
			delete *iter;
		colliders.clear();

		colliders.push_back(collider);
		localCentroid = collider->GetPosition();
		inverseMass = gFloat(0.0f);
		inverseInertiaTensor = Matrix::INFINITE_MASS_INERTIA_TENSOR;
		CalcDerivedData();
		SetAwake(false);	// Infinite Mass can't move by definition, so set to asleep for now
		return;
	}

	colliders.push_back(collider); 

	// Reset Centroid and mass
	localCentroid.Zero();
	inverseMass = 0.0f;
#ifdef TRACK_MASS
	mass = 0.0f;
#else
	gFloat mass = 0.0f;
#endif

	// Calculate local Centroid and mass
	gFloat cMass;
	for(auto iter = colliders.begin(); iter != colliders.end(); ++iter)
	{
		cMass = collider->GetMass();

		// accumulate mass and weighted contribution to Centroid
		mass += cMass;
		centroid += collider->GetPosition() * cMass;
	}

	// Calculate inverse mass and Centroid
	inverseMass = gFloat(1.0f) / mass;
	localCentroid *= inverseMass;

	// Calculate local inertia tensor
	Matrix inertia, offset, tensorProd;
	Vector r;
	gFloat rDotR;
	inertia.Zero();
	for(auto iter = colliders.begin(); iter != colliders.end(); ++iter)
	{
		offset = collider->GetOffset();
		r = Vector(offset(3,0), offset(3,1), offset(3,2));
		rDotR = r.DotProduct(r);
		tensorProd = r.TensorProduct(r);

		// Accumulate local inertia tensor contribution using Parallel Axis Theorem
		//	(Moment of Inertia around axis 'b' parallel to axis through center of mass 'a' when 'b' is 'd' units from 'a'
		//			= MomentofInertia around 'a' + mass * d^2
		inertia += collider->GetInertiaTensor() + (((Matrix()*rDotR) - tensorProd) * collider->GetMass());
		inertia(3,3) = 1.0f;
	}

	// Calculate Inverse Inertia Tensor
	inverseInertiaTensor = inertia.Inverse3();

	CalcDerivedData();
	SetAwake(true);
}

void RigidBody::LoadMesh(SmartPointer<MeshData> meshData)
{
	shaderResource = GraphicsLocator::GetGraphics()->CreateBuffers(meshData);
	highlightColor = meshData->color;	// set hightlight color to default mesh color
	shaderResource->world = &transformationMatrix;
	shaderResource->color = &highlightColor;
}

void RigidBody::LoadMesh(std::string& meshName)
{
	SmartPointer<MeshData> md = MeshData::FindResourceByName(meshName);
	shaderResource = GraphicsLocator::GetGraphics()->CreateBuffers(md);
	highlightColor = md->color;		// set hightlight color to default mesh color
	shaderResource->world = &transformationMatrix;
	shaderResource->color = &highlightColor;
}

void RigidBody::ApplyForceAtPoint(const Vector& f, const Vector& p)
{
	force += f;
	torque += (p - centroid).CrossProduct(f);
	isAwake = true;
}

void RigidBody::ApplyForceAtLocalPoint(const Vector& f, const Vector& p)
{
	force += f;
	torque += (GetPointInWorldSpace(p) - centroid).CrossProduct(f);
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

unsigned int RigidBody::GetColliders(std::vector<Collider*>& c) { c = colliders; return colliders.size(); }

Vector RigidBody::GetVelocity() const { return velocity; }
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
Vector RigidBody::GetAngularVelocity() const { return angularVelocity; }
void RigidBody::SetAngularVelocity(const Vector& v)
{
	if(!(properties & OVERRIDE_ANGULAR_VELOCITY)) return;
	angularVelocity = v;
}
void RigidBody::SetAngularVelocity(const gFloat x, const gFloat y, const gFloat z)
{
	if(!(properties & OVERRIDE_ANGULAR_VELOCITY)) return;
	angularVelocity.x = x;
	angularVelocity.y = y;
	angularVelocity.z = z;
}
Vector RigidBody::GetAngularAcceleration() const { return angularAcceleration; }
void RigidBody::SetAngularAcceleration(const Vector& v)
{
	if(!(properties & OVERRIDE_ANGULAR_ACCELERATION)) return;
	angularAcceleration = v;
}
void RigidBody::SetAngularAcceleration(const gFloat x, const gFloat y, const gFloat z)
{
	if(!(properties & OVERRIDE_ANGULAR_ACCELERATION)) return;
	angularAcceleration.x = x;
	angularAcceleration.y = y;
	angularAcceleration.z = z;
}

Quaternion RigidBody::GetOrientation() const { return orientation; }

Matrix RigidBody::GetInverseInertiaTensorWorld() const { return inverseInertiaTensorWorld; }
void RigidBody::GetInverseInertiaTensorWorld(Matrix* m) const { *m = inverseInertiaTensorWorld; }

Vector RigidBody::GetLastFrameAcceleration() const { return lastAcceleration; }
void RigidBody::ForceSetPosition(const Vector& p) { position = p; UpdateCentroidFromPosition(); }
void RigidBody::ForceSetCentroid(const Vector& c) { centroid = c; UpdatePositionFromCentroid(); }
void RigidBody::ForceSetOrientation(const Quaternion& o) { orientation = o; }
void RigidBody::ForceSetVelocity(const Vector& v) { velocity = v; }
void RigidBody::ForceSetAngularVelocity(const Vector& v) { angularVelocity = v; }
void RigidBody::ForceSetAcceleration(const Vector& a) { acceleration = a; }
void RigidBody::ForceSetAngularAcceleration(const Vector& a) { angularAcceleration = a; }
void RigidBody::ForceAddPosition(const Vector& p) { position += p; centroid += p; }
void RigidBody::ForceAddCentroid(const Vector& c) { centroid += c; position += c; }
void RigidBody::ForceAddOrientation(const Vector& o) { /*orientation += (orientation * (Quaternion(o.x,o.y,o.z,0.0f) * 0.5f));*/
orientation.AddRotation(o, 1);}
void RigidBody::ForceAddVelocity(const Vector& v) {	velocity += v; }
void RigidBody::ForceAddAngularVelocity(const Vector& v) { angularVelocity += v; }
void RigidBody::ForceAddAcceleration(const Vector& a) { acceleration += a; }
void RigidBody::ForceAddAngularAcceleration(const Vector& a) { angularAcceleration += a; }
void RigidBody::SetSolved() { solved = true; }