#include "Contact.h"

using namespace Glade;

Contact::Contact() : b1(nullptr), b2(nullptr), coeffRestitution(0.0f), penetrationDepth(0.0f) { }
Contact::Contact(RigidBody* b1_, RigidBody* b2_, gFloat res, gFloat f, Vector n, Vector p, gFloat pen) : b1(b1_), b2(b2_), coeffRestitution(res), friction(f), normal(n), point(p), penetrationDepth(pen)
{
}

void Contact::SetNewContact(RigidBody* b1_, RigidBody* b2_, gFloat res, gFloat f, Vector n, Vector p, gFloat pen)
{
	b1 = b1_;
	b2 = b2_;
	coeffRestitution = res;
	friction = f;
	normal = n;
	point = p;
	penetrationDepth = pen;
}

void Contact::ResolveImpulse(Vector (&deltaVel)[2], Vector (&deltaAngVel)[2])
{
	Vector contactImpulse;
	if(friction == (gFloat)0.0f)
		contactImpulse = CalcImpulseNoFriction();
	else
		contactImpulse = CalcImpulseFriction();

	// Convert impulse to World space
	Vector impulse = contactImpulse * contactToWorld;

	// Splie impulse into linear and rotational components
	Vector impulsiveTorque = b1ContactPoint.CrossProduct(impulse);
	deltaAngVel[0] = impulsiveTorque * b1->GetInverseInertiaTensorWorld();
	deltaVel[0] = impulse * b1->GetInverseMass();

	// Apply
	b1->ForceAddVelocity(deltaVel[0]);
	b1->ForceAddAngularVelocity(deltaAngVel[0]);

	if(b2 != nullptr)
	{
		impulsiveTorque = b2ContactPoint.CrossProduct(impulse);
		deltaAngVel[1] = impulsiveTorque * b2->GetInverseInertiaTensorWorld();
		deltaVel[1] = impulse * -b2->GetInverseMass();

		b2->ForceAddVelocity(deltaVel[1]);
		b2->ForceAddAngularVelocity(deltaAngVel[1]);
	}
}

void Contact::ResolveInterpenetration(Vector (&deltaPos)[2], Vector (&deltaOrient)[2], gFloat pen)
{
	const gFloat angularLimit = (gFloat)0.2f;
	gFloat angularMove[2];
	gFloat linearMove[2];
	gFloat totalInertia = 0;
	gFloat linearInertia[2];
	gFloat angularInertia[2];

	// Work out inertia of each RigidBody due to angular inertia in direction of Contact normal
	RigidBody* bodies[2] = { b1, b2 };
	Vector relativeContactPoint[2] = { b1ContactPoint, b2ContactPoint };
	for(unsigned int i = 0; i < 2; ++i) if(bodies[i])
	{
		Matrix inverseInertiaTensor = bodies[i]->GetInverseInertiaTensorWorld();

		// Use same procedure for calculating frictionless velocity change to work out angular inertia
		Vector angularInertiaWorld = relativeContactPoint[i].CrossProduct(normal);
		angularInertiaWorld *= inverseInertiaTensor;
		angularInertiaWorld = angularInertiaWorld.CrossProduct(relativeContactPoint[i]);
		angularInertia[i] = angularInertiaWorld.DotProduct(normal);

		// Linear component is simply inverse mass
		linearInertia[i] = bodies[i]->GetInverseMass();

		// Track total inertia from all components
		totalInertia += linearInertia[i] + angularInertia[i];
	}

	// Calculate and apply changes
	for(unsigned int i = 0; i < 2; ++i) if(bodies[i])
	{
		gFloat sign = (i == 0) ? -1 : 1;

		angularMove[i] = sign * pen * (angularInertia[i] / totalInertia);
		linearMove[i] = sign * pen * (linearInertia[i] / totalInertia);

		// Limit angular move to avoid angular projections that are too great
		// (when mass is large, but inertia tensor is small)
		Vector projection = relativeContactPoint[i] + (normal * (-relativeContactPoint[i].DotProduct(normal)));

		// Use small angle approximation for the sine of the angle
		// (i.e. magnitude would be sin(angularLimit) * projection.Magitude(), but we 
		//		approximate sin(angularLimit) to angularLimit
		gFloat maxMagnitude = angularLimit * projection.Magnitude();

		if(angularMove[i] < -maxMagnitude)
		{
			gFloat totalMove = angularMove[i] + linearMove[i];
			angularMove[i] = -maxMagnitude;
			linearMove[i] = totalMove - angularMove[i];
		}
		else if(angularMove[i] > maxMagnitude)
		{
			gFloat totalMove = angularMove[i] + linearMove[i];
			angularMove[i] = maxMagnitude;
			linearMove[i] = totalMove - angularMove[i];
		}

		// We have linear amount of movement required by turning the RigidBody in angularMove[i]
		// We now need to calculate desired rotation to achieve that
		if(angularMove[i] == 0)
			deltaOrient[i].Zero();
		else // Work out the direction to rotate
			deltaOrient[i] = (relativeContactPoint[i].CrossProduct(normal) * bodies[i]->GetInverseInertiaTensorWorld()) * (angularMove[i] / angularInertia[i]);
		
		// Velocity change is just linear movement along Contact normal
		deltaPos[i] = normal * linearMove[i];

		// Apply
		bodies[i]->ForceAddPosition(deltaPos[i]);
		bodies[i]->ForceAddOrientation(deltaOrient[i]);

		// Recalculate derived data for sleeping RigidBodies now that the changes are applied
		//  (Awake bodies will automatically do this after Integration)
		if(!bodies[i]->GetAwake())
			bodies[i]->CalcDerivedData();
	}
}


void Contact::CalculateInternals()
{
	if(b1 == nullptr) ReverseContact();

	CalculateContactBasis();

	// Calculate relative position of Contact Point to each RigidBody
	b1ContactPoint = point - b1->GetPosition();
	if(b2 != nullptr)	b2ContactPoint = point - b2->GetPosition();

	// Calculate relative velocity of RigidBodies at Contact Point
	relativeVelocity = CalculateLocalVelocity(1);
	if(b2 != nullptr) relativeVelocity -= CalculateLocalVelocity(2);

	// Calculate desired change in velocity to resolve Contact
	CalculateDesiredDeltaVelocity();
}

inline
void Contact::CalculateContactBasis()
{
	Vector tangent1, tangent2;
	if(Abs(normal.x) > Abs(normal.y))
	{
		// Scaling factor to ensure results are normalized
		const gFloat s = (gFloat)1.0f / Sqrt(normal.z*normal.z + normal.x*normal.x);

		// New Z-axis - at a right angle to world Y-axis
		tangent1.x = normal.z * s;
		tangent1.y = 0;
		tangent1.z = -normal.x * s;

		// New Y-Axis - at a right angle to new X- and Z- axes
		tangent2.x = normal.y * tangent1.x;
		tangent2.y = normal.z * tangent1.x - normal.x * tangent1.z;
		tangent2.z = -normal.y * tangent1.x;
	}
	else
	{
		const gFloat s = (gFloat)1.0f / Sqrt(normal.z*normal.z + normal.y*normal.y);

		// New Z-axis - at a right angle to World X-axis
		tangent1.x = 0;
		tangent1.y = -normal.z * s;
		tangent1.z = normal.y * s;

		// New Y-axis - at a right angle to new X- and Z- axes
		tangent2.x = normal.y * tangent1.z - normal.z * tangent1.y;
		tangent2.y = -normal.x * tangent1.z;
		tangent2.z = normal.x * tangent1.y;
	}
	contactToWorld.SetBasis(normal, tangent2, tangent1);
}

Vector Contact::CalculateLocalVelocity(unsigned int body)
{
	RigidBody* rb = ((body == 1) ? b1 : b2);

	// Get linear velocity of Contact point
	Vector velocity = rb->GetVelocity() +
					rb->GetAngularVelocity().CrossProduct(((body == 1) ? b1ContactPoint : b2ContactPoint));

	// Turn velocity into contact-coordinates
	Vector contactVelocity = velocity * contactToWorld.Transpose();

	// Get velocity due to acceleration this frame
	Vector accVelocity = rb->GetLastFrameAcceleration() * PHYSICS_TIMESTEP;
	accVelocity *= contactToWorld.Transpose();

	// Ignore x-component because it is in direction of Contact normal
	// (In ContactBasis, the x-axis is set to be the direction of the Contact normal)
	accVelocity.x = 0;
	
	// 
	return contactVelocity + accVelocity;
}

void Contact::CalculateDesiredDeltaVelocity()
{
	const static gFloat velocityLimit = (gFloat)0.25f;

	// Calculate acceleration induced velocity accumulated this frame
	gFloat velocityFromAccel = (gFloat)0.0f;

	if(b1->GetAwake())
		velocityFromAccel += (b1->GetLastFrameAcceleration() * PHYSICS_TIMESTEP).DotProduct(normal);
	if(b2 != nullptr && b2->GetAwake())
		velocityFromAccel -= (b2->GetLastFrameAcceleration() * PHYSICS_TIMESTEP).DotProduct(normal);

	// If the veloctiy is very low, limit restitution
	gFloat rest = coeffRestitution;
	if(Abs(relativeVelocity.x) < velocityLimit)
		rest = (gFloat)0.0f;

	desiredDeltaVel = -relativeVelocity.x - (rest * (relativeVelocity.x - velocityFromAccel));
}


Vector Contact::CalcImpulseNoFriction()
{
	// Calc change in velocity in world space per unit of impulse
	Vector deltaVelWorld = b1ContactPoint.CrossProduct(normal);	// torque per unit of impulse
	deltaVelWorld *= b1->GetInverseInertiaTensorWorld();		// angular velocity per unit of impulse
	deltaVelWorld = deltaVelWorld.CrossProduct(b1ContactPoint);	// velocity per unit of impulse

	// Get change in velocity in Contact coordinates
	gFloat deltaVel = deltaVelWorld.DotProduct(normal);

	// Add linear component of velocity change
	deltaVel += b1->GetInverseMass();

	if(b2 != nullptr)
	{
		deltaVelWorld = b2ContactPoint.CrossProduct(normal);		// torque per unit of impulse
		deltaVelWorld *= b2->GetInverseInertiaTensorWorld();		// angular velocity per unit of impulse
		deltaVelWorld = deltaVelWorld.CrossProduct(b2ContactPoint);	// velocity per unit of impulse
	//	deltaVelWorld = normal.CrossProduct(b2ContactPoint);
	//	deltaVelWorld *= b2->GetInverseInertiaTensorWorld();
	//	deltaVelWorld = b2ContactPoint.CrossProduct(deltaVelWorld);


		deltaVel += deltaVelWorld.DotProduct(normal);
		deltaVel += b2->GetInverseMass();
	}

	return Vector(desiredDeltaVel / deltaVel, 0.0f, 0.0f); 
}

Vector Contact::CalcImpulseFriction()
{
	gFloat inverseMass = b1->GetInverseMass();

	// Built matrix to convert Contact impulse to change in velocity in world coordinates
	Matrix impulseToTorque = Matrix::SkewSymmetricMatrix(b1ContactPoint);
	Matrix deltaVelWorld = impulseToTorque;
	deltaVelWorld *= b1->GetInverseInertiaTensorWorld();
	deltaVelWorld *= impulseToTorque;
	deltaVelWorld *= -1;

	if(b2 != nullptr)
	{
		impulseToTorque = Matrix::SkewSymmetricMatrix(b2ContactPoint);

		Matrix deltaVelWorld2 = impulseToTorque;
		deltaVelWorld2 *= b2->GetInverseInertiaTensorWorld();
		deltaVelWorld2 *= impulseToTorque;
		deltaVelWorld2 *= -1;

		// Add to total delta velocity
		deltaVelWorld += deltaVelWorld2;

		// Add to total inverse mass
		inverseMass += b2->GetInverseMass();
	}

	// Change of basis to Contact space
	Matrix deltaVelocity = contactToWorld.Transpose();
	deltaVelocity *= deltaVelWorld;
	deltaVelocity *= contactToWorld;

	// Add in linear velocity change
	deltaVelocity(0,0) += inverseMass;
	deltaVelocity(1,1) += inverseMass;
	deltaVelocity(2,2) += inverseMass;

	// Invert to get impulse needed per unit velocity
	Matrix impulseMatrix = deltaVelocity.Inverse3();

	// Find the target velocities to kill
	Vector velKill(desiredDeltaVel, -relativeVelocity.y, -relativeVelocity.z);

	// Find the impulseto kill target velocities
	Vector contactImpulse = velKill * impulseMatrix;

	// Check for exceeding friction
	gFloat planarImpulse = Sqrt(contactImpulse.y*contactImpulse.y + contactImpulse.z*contactImpulse.z);
	if(planarImpulse > contactImpulse.x * friction)
	{
		contactImpulse.y /= planarImpulse;
		contactImpulse.z /= planarImpulse;

		contactImpulse.x = deltaVelocity(0,0) +
					(deltaVelocity(0,1) * friction * contactImpulse.y) +
					(deltaVelocity(0,2) * friction * contactImpulse.z);
		contactImpulse.x = desiredDeltaVel / contactImpulse.x;
		contactImpulse.y *= friction * contactImpulse.x;
		contactImpulse.z *= friction * contactImpulse.x;
	}

	return contactImpulse;
}

void Contact::MatchAwakeState()
{
	// Collision with world will never cause a RigidBody to wake up
	if(b2 == nullptr) return;

	bool b1Awake = b1->GetAwake();
	bool b2Awake = b2->GetAwake();

	// Wake up the sleeping one
	if(b1Awake ^ b2Awake)
	{
		if(b1Awake)	b2->SetAwake();
		else		b1->SetAwake();
	}
}

void Contact::ReverseContact()
{
	normal *= (gFloat)-1.0f;
	RigidBody* temp = b1;
	b1 = b2;
	b2 = temp;
}