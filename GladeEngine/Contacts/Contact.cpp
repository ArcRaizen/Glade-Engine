#include "Contact.h"

using namespace Glade;

Contact::Contact() : b1(nullptr), b2(nullptr), coeffRestitution(0.0f), penetrationDepth(0.0f) { }
Contact::Contact(RigidBody* b1_, RigidBody* b2_, gFloat res, gFloat sMu, gFloat dMu, Vector n, Vector p, gFloat pen) : b1(b1_), b2(b2_), coeffRestitution(res), staticFriction(sMu), dynamicFriction(dMu), normal(n), point(p), penetrationDepth(pen)
{
}

void Contact::SetNewContact(RigidBody* b1_, RigidBody* b2_, gFloat res, gFloat sMu, gFloat dMu, Vector n, Vector p, gFloat pen)
{
	b1 = b1_;
	b2 = b2_;
	coeffRestitution = res;
	staticFriction = sMu;
	dynamicFriction = dMu;
	normal = n;
	point = p;
	penetrationDepth = pen;

	GraphicsLocator::GetDebugGraphics()->PushLine(point - (normal * gFloat(5.f)), point + (normal * gFloat(5.f)), DebugDraw::Color(1,0,0,1));
}

void Contact::ResolveImpulse(Vector (&deltaVel)[2], Vector (&deltaAngVel)[2])
{
	Vector contactImpulse;
	Matrix inverseInertiaTensorWorld[2];
	b1->GetInverseInertiaTensorWorld(&inverseInertiaTensorWorld[0]);
	if(b2 != nullptr) b2->GetInverseInertiaTensorWorld(&inverseInertiaTensorWorld[1]);

	if(staticFriction == gFloat(0.0f))	// static always larger than dynamic, so only need to check if static is 0
		contactImpulse = CalcImpulseNoFriction(inverseInertiaTensorWorld);
	else
		contactImpulse = CalcImpulseFriction(inverseInertiaTensorWorld);

	// Convert impulse to World space
	Vector impulse = contactImpulse * contactToWorld;

	ResolveImpulse2(deltaVel, deltaAngVel);

	// Split impulse into linear and rotational components
	Vector impulsiveTorque = b1ContactPoint.CrossProduct(impulse);
//	Vector impulsiveTorque = impulse.CrossProduct(b1ContactPoint);
	deltaAngVel[0] = impulsiveTorque * inverseInertiaTensorWorld[0];
//	deltaAngVel[0] *= DEG2RAD;
	deltaVel[0] = impulse * b1->GetInverseMass();

	// Apply
	b1->ForceAddVelocity(deltaVel[0]);
	b1->ForceAddAngularVelocity(deltaAngVel[0]);

	if(b2 != nullptr)
	{
		//impulsiveTorque = b2ContactPoint.CrossProduct(impulse);
		impulsiveTorque = impulse.CrossProduct(b2ContactPoint);
		deltaAngVel[1] = impulsiveTorque * inverseInertiaTensorWorld[1];
		deltaVel[1] = impulse * -b2->GetInverseMass();

		b2->ForceAddVelocity(deltaVel[1]);
		b2->ForceAddAngularVelocity(deltaAngVel[1]);
	}

	if(b1->GetInverseMass() == 0 || (b2 != nullptr) && b2->GetInverseMass() != 0)
	{
		b1->SetSolved();
		b2->SetSolved();
	}
}

void Contact::ResolveImpulse2(Vector (&deltaVel)[2], Vector (&deltaAngVel)[2])
{
	Vector j = (relativeVelocity * -(1.0f + coeffRestitution)) / 
			(b1->GetInverseMass() + b2->GetInverseMass() + 
			((b1ContactPoint.CrossProduct(normal)*b1->GetInverseInertiaTensorWorld()).CrossProduct(b1ContactPoint) + 
			(b2ContactPoint.CrossProduct(normal)*b2->GetInverseInertiaTensorWorld()).CrossProduct(b2ContactPoint)).DotProduct(normal)
			);
	
	Vector tangent = (relativeVelocity - normal * (relativeVelocity.DotProduct(normal))).Normalized();
	Vector impulseVec = normal * j.DotProduct(normal);
	Vector frictionVec = tangent * j.DotProduct(tangent);
	gFloat frictionMax = impulseVec.Magnitude() * staticFriction;
	if(frictionVec.Magnitude() > frictionMax)
	{
		frictionVec.NormalizeInPlace();
		// gFloat mu = Sqrt(b1->DynamicFriction^2 + b2->DynamicFriction^2)
		frictionVec *= impulseVec.Magnitude() * dynamicFriction;
	}

	j = frictionVec + -impulseVec;
// ~~~~ NO FRICTION ~~~~
	deltaVel[0] = (j) * b1->GetInverseMass();
	deltaVel[1] = (j) * b2->GetInverseMass();
	deltaAngVel[0] = (b1ContactPoint.CrossProduct(j)) * b1->GetInverseInertiaTensorWorld();
	deltaAngVel[1] = (b2ContactPoint.CrossProduct(j)) * b2->GetInverseInertiaTensorWorld();
// ~~~~~~~~~~~~~~~~~~~~~




//	b1->ForceAddVelocity(deltaVel[0]);
//	b1->ForceAddAngularVelocity(deltaAngVel[0]);
//	b2->ForceAddVelocity(deltaVel[1]);
//	b2->ForceAddAngularVelocity(deltaAngVel[1]);
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
//		Vector angularInertiaWorld = normal.CrossProduct(relativeContactPoint[i]);
//		angularInertiaWorld *= inverseInertiaTensor;
//		angularInertiaWorld = relativeContactPoint[i].CrossProduct(angularInertiaWorld);
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
		
		// Position change is just linear movement along Contact normal
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

void Contact::CalculateInertia()
{
	// Body 1 Inertia
	// Work out inertia of RigidBody due to angular inertia in direction of Contact normal
	Vector angularInertiaWorld = b1ContactPoint.CrossProduct(normal);
	angularInertiaWorld *= b1->GetInverseInertiaTensorWorld();
	angularInertiaWorld = angularInertiaWorld.CrossProduct(b1ContactPoint);
	angularInertia[0] = angularInertiaWorld.DotProduct(normal);
	linearInertia[0] = b1->GetInverseMass();		// Linear component is simply inverse mass
		
	// Body 2 Inertia
	angularInertiaWorld = b2ContactPoint.CrossProduct(normal);
	angularInertiaWorld *= b2->GetInverseInertiaTensorWorld();
	angularInertiaWorld = angularInertiaWorld.CrossProduct(b2ContactPoint);
	angularInertia[1] = angularInertiaWorld.DotProduct(normal);
	linearInertia[1] = b2->GetInverseMass();
}

void Contact::CalculatePenetrationResolution(Vector (&deltaPos)[2], Vector (&deltaOrient)[2], gFloat pen)
{
	const gFloat angularLimit = gFloat(0.2f);
	gFloat angularMove[2];
	gFloat linearMove[2];
	RigidBody* bodies[2] = { b1, b2 };
	Vector relativeContactPoint[2] = { b1ContactPoint, b2ContactPoint };
	
	// Total Inertia of RigidBodies is simply sum of their linear and angular inertia
	gFloat totalInertia = linearInertia[0] + linearInertia[1] + angularInertia[0] + angularInertia[1];

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
		if(angularMove[i] != gFloat(0.0f))
			deltaOrient[i] = (relativeContactPoint[i].CrossProduct(normal) * bodies[i]->GetInverseInertiaTensorWorld()) * (angularMove[i] / angularInertia[i]);
		
		// Position change is just linear movement along Contact normal
		deltaPos[i] = normal * linearMove[i];

		/* DO NOT DO THIS UNTIL ALL RESOLUTIONS ARE CALCULATED
		// Apply
		bodies[i]->ForceAddPosition(deltaPos[i]);
		bodies[i]->ForceAddOrientation(deltaOrient[i]);

		// Recalculate derived data for sleeping RigidBodies now that the changes are applied
		//  (Awake bodies will automatically do this after Integration)
		if(!bodies[i]->GetAwake())
			bodies[i]->CalcDerivedData();
		*/
	}
}

// Assuming B2 is Infinite Mass/Inertia because it has already been solved
void Contact::CalculatePenetrationResolutionB1(Vector& deltaPos, Vector& deltaOrient, gFloat pen)
{
	const gFloat angularLimit = gFloat(0.2f);
	
	// Total Inertia of RigidBody is sum of linear and angular inertia
	gFloat totalInertia = linearInertia[0] + angularInertia[0];

	// Calculate and apply changes
	gFloat angularMove = gFloat(-1.0f) * pen * (angularInertia[0] / totalInertia);
	gFloat linearMove = gFloat(-1.0f) * pen * (linearInertia[0] / totalInertia);

	// Limit angular move to avoid angular projections that are too great
	//	(when mass is large, but inertia tensor is small)
	Vector projection = b1ContactPoint + (normal * (-b1ContactPoint.DotProduct(normal)));

	// Use small angle approximation for the sine of the angle
	// (i.e. magnitude would be sine(angularLimit) * projection.Magnitude(), but we
	//		approximate sin(angularLimit) to angularLimit
	gFloat maxMagnitude = angularLimit * projection.Magnitude();

	if(angularMove < -maxMagnitude)
	{
		gFloat totalMove = angularMove + linearMove;
		angularMove = -maxMagnitude;
		linearMove = totalMove - angularMove;
	}
	else if(angularMove > maxMagnitude)
	{
		gFloat totalMove = angularMove + linearMove;
		angularMove = maxMagnitude;
		linearMove = totalMove - angularMove;
	}

	// We have linear amount of movement require by turning the RigidBody in angularMove
	// We node need to calculate desired rotation to achieve that
	if(angularMove != gFloat(0.0f))
		deltaOrient = (b1ContactPoint.CrossProduct(normal) * b1->GetInverseInertiaTensorWorld()) * (angularMove / angularInertia[0]);

	// Position change is just linear movement along Contact normal
	deltaPos = normal * linearMove;

	// Apply
	b1->ForceAddPosition(deltaPos);
	b1->ForceAddOrientation(deltaOrient);

	// Recalculate derived data fro sleeping RigidBodies now that the changes are applied
	//	(Awake bodies will automatically do this after integration)
	if(!b1->GetAwake())
		b1->CalcDerivedData();
}

void Contact::CalculatePenetrationResolutionB2(Vector& deltaPos, Vector& deltaOrient, gFloat pen)
{
	const gFloat angularLimit = gFloat(0.2f);
	
	// Total Inertia of RigidBody is sum of linear and angular inertia
	gFloat totalInertia = linearInertia[1] + angularInertia[1];

	// Calculate and apply changes
	gFloat angularMove = pen * (angularInertia[1] / totalInertia);
	gFloat linearMove = pen * (linearInertia[1] / totalInertia);

	// Limit angular move to avoid angular projections that are too great
	//	(when mass is large, but inertia tensor is small)
	Vector projection = b2ContactPoint + (normal * (-b2ContactPoint.DotProduct(normal)));

	// Use small angle approximation for the sine of the angle
	// (i.e. magnitude would be sine(angularLimit) * projection.Magnitude(), but we
	//		approximate sin(angularLimit) to angularLimit
	gFloat maxMagnitude = angularLimit * projection.Magnitude();

	if(angularMove < -maxMagnitude)
	{
		gFloat totalMove = angularMove + linearMove;
		angularMove = -maxMagnitude;
		linearMove = totalMove - angularMove;
	}
	else if(angularMove > maxMagnitude)
	{
		gFloat totalMove = angularMove + linearMove;
		angularMove = maxMagnitude;
		linearMove = totalMove - angularMove;
	}

	// We have linear amount of movement require by turning the RigidBody in angularMove
	// We node need to calculate desired rotation to achieve that
	if(angularMove != gFloat(0.0f))
		deltaOrient = (b2ContactPoint.CrossProduct(normal) * b2->GetInverseInertiaTensorWorld()) * (angularMove / angularInertia[0]);

	// Position change is just linear movement along Contact normal
	deltaPos = normal * linearMove;

	// Apply
	b2->ForceAddPosition(deltaPos);
	b2->ForceAddOrientation(deltaOrient);

	// Recalculate derived data fro sleeping RigidBodies now that the changes are applied
	//	(Awake bodies will automatically do this after integration)
	if(!b2->GetAwake())
		b2->CalcDerivedData();
}


void Contact::CalculateInternals()
{
	if(b1 == nullptr) ReverseContact();

	CalculateContactBasis();

	// Calculate relative position of Contact Point to each RigidBody
	b1ContactPoint = (point - b1->GetCentroid()).Cleanse();
	if(b2 != nullptr)	b2ContactPoint = (point - b2->GetCentroid()).Cleanse();

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
	if(Abs(normal.y) > Abs(normal.x))
	{	// Assuming world X-Axis as second axis
		const gFloat s = (gFloat)1.0f / Sqrt(normal.z*normal.z + normal.y*normal.y);
		
		// New Z-Axis (z = x.Cross(y))
		tangent2.x = 0;
		tangent2.y = -normal.z * s;
		tangent2.z = normal.y * s;

		// New X-Axis (x = y.Cross(z))
		tangent1.x = normal.y * tangent2.z;
		tangent1.y = normal.z * tangent2.x - normal.x*tangent2.z;
		tangent1.z = -normal.y * tangent2.x;
	}
	else
	{	// Assuming world Z-Axis as second axis
		const gFloat s = (gFloat)1.0f / Sqrt(normal.z*normal.z + normal.x*normal.x);

		// New Z-Axis (z = x.Cross(y))
		tangent2.x = -normal.z * s;
		tangent2.y = 0;
		tangent2.z = normal.x * s;

		// New X-Axis (x = (y.Cross(z))
		tangent1.x = normal.y * tangent2.z;
		tangent1.y = normal.z * tangent2.x - normal.x * tangent2.z;
		tangent1.z = -normal.y * tangent2.x;
	}
	contactToWorld.SetBasis(tangent1, normal, tangent2);
}

Vector Contact::CalculateLocalVelocity(unsigned int body)
{
	RigidBody* rb = ((body == 1) ? b1 : b2);

	// Get linear velocity of Contact point
	Vector velocity = rb->GetVelocity() +
				rb->GetAngularVelocity().CrossProduct(((body == 1) ? b1ContactPoint : b2ContactPoint));
				//((body == 1) ? b1ContactPoint.CrossProduct(rb->GetAngularVelocity()) :
				//b2ContactPoint.CrossProduct(rb->GetAngularVelocity()));

	// Turn velocity into contact-coordinates
	Vector contactVelocity = contactToWorld.Transpose3Times(velocity);

	// Get velocity due to acceleration this frame
	Vector accVelocity = rb->GetLastFrameAcceleration() * PHYSICS_TIMESTEP;
	accVelocity *= contactToWorld.Transpose3();

	// Ignore y-component because it is in direction of Contact normal
	// (In ContactBasis, the y-axis is set to be the direction of the Contact normal)
	accVelocity.y = 0;
	
	// 
	return contactVelocity + accVelocity;
}

void Contact::CalculateDesiredDeltaVelocity()
{
//	const static gFloat velocityLimit = (gFloat)0.25f;

	// Calculate acceleration induced velocity accumulated this frame
	gFloat velocityFromAccel = (gFloat)0.0f;

	if(b1->GetAwake())
		velocityFromAccel += (b1->GetLastFrameAcceleration() * PHYSICS_TIMESTEP).DotProduct(normal);
	if(b2 != nullptr && b2->GetAwake())
		velocityFromAccel -= (b2->GetLastFrameAcceleration() * PHYSICS_TIMESTEP).DotProduct(normal);

	// If the veloctiy is very low, limit restitution
	gFloat rest = coeffRestitution;
	if(relativeVelocity.y < (gFloat)0.25f)
		rest = (gFloat)0.0f;

	desiredDeltaVel = -relativeVelocity.y - (rest * (relativeVelocity.y - velocityFromAccel));
}


Vector Contact::CalcImpulseNoFriction(Matrix (&inverseInertiaTensorWorld)[2])
{
	// Calc change in velocity in world space per unit of impulse
//	Vector deltaVelWorld = b1ContactPoint.CrossProduct(normal);	// torque per unit of impulse
//	deltaVelWorld *= b1->GetInverseInertiaTensorWorld();		// angular velocity per unit of impulse
//	deltaVelWorld = deltaVelWorld.CrossProduct(b1ContactPoint);	// velocity per unit of impulse
	Vector deltaVelWorld = normal.CrossProduct(b1ContactPoint);
	deltaVelWorld *= inverseInertiaTensorWorld[0];
	deltaVelWorld = b1ContactPoint.CrossProduct(deltaVelWorld);

	// Get change in velocity in Contact coordinates
	gFloat deltaVel = deltaVelWorld.DotProduct(normal);

	// Add linear component of velocity change
	deltaVel += b1->GetInverseMass();

	if(b2 != nullptr)
	{
		deltaVelWorld = b2ContactPoint.CrossProduct(normal);		// torque per unit of impulse
		deltaVelWorld *= inverseInertiaTensorWorld[1];				// angular velocity per unit of impulse
		deltaVelWorld = deltaVelWorld.CrossProduct(b2ContactPoint);	// velocity per unit of impulse
//		deltaVelWorld = normal.CrossProduct(b2ContactPoint);
//		deltaVelWorld *= b2->GetInverseInertiaTensorWorld();
//		deltaVelWorld = b2ContactPoint.CrossProduct(deltaVelWorld);


		deltaVel += deltaVelWorld.DotProduct(normal);
		deltaVel += b2->GetInverseMass();
	}

	return Vector(0.0f, desiredDeltaVel / deltaVel, 0.0f); 
}

Vector Contact::CalcImpulseFriction(Matrix (&inverseInertiaTensorWorld)[2])
{
	// Built matrix to convert Contact impulse to change in velocity in world coordinates
	Matrix impulseToTorque = Matrix::SkewSymmetricMatrix(b1ContactPoint);
	Matrix deltaVelWorld = impulseToTorque;
	deltaVelWorld *= inverseInertiaTensorWorld[0];// * deltaVelWorld;
	deltaVelWorld *= impulseToTorque;// * deltaVelWorld;
	deltaVelWorld *= -1;

	gFloat inverseMass = b1->GetInverseMass();

	if(b2 != nullptr)
	{
		impulseToTorque = Matrix::SkewSymmetricMatrix(b2ContactPoint);

		Matrix deltaVelWorld2 = impulseToTorque;
		deltaVelWorld2 *= inverseInertiaTensorWorld[1];// * deltaVelWorld2;
		deltaVelWorld2 *= impulseToTorque;// * deltaVelWorld2;
		deltaVelWorld2 *= -1;

		// Add to total delta velocity
		deltaVelWorld += deltaVelWorld2;
		deltaVelWorld(3,3) = 0;

		// Add to total inverse mass
		inverseMass += b2->GetInverseMass();
	}

	// Change of basis to Contact space
	Matrix deltaVelocity = (contactToWorld.Times3(deltaVelWorld)).TimesTranspose3(contactToWorld);
//	Matrix deltaVelocity = (contactToWorld.Transpose3Times(deltaVelWorld)).Times3(contactToWorld);
//	Matrix deltaVelocity = deltaVelWorld;

//	Matrix deltaVelocity = contactToWorld.Transpose3();
//	deltaVelocity *= deltaVelWorld;
//	deltaVelocity *= contactToWorld;

	// Add in linear velocity change
	deltaVelocity(0,0) += inverseMass;
	deltaVelocity(1,1) += inverseMass;
	deltaVelocity(2,2) += inverseMass;

	// Invert to get impulse needed per unit velocity
	Matrix impulseMatrix = deltaVelocity.Inverse4();

	// Find the target velocities to kill
	Vector velKill(relativeVelocity.x, desiredDeltaVel, relativeVelocity.z);

	// Find the impulse to kill target velocities
	Vector contactImpulse = velKill * impulseMatrix;

	// Check for exceeding friction
	gFloat planarImpulse = Sqrt(contactImpulse.x*contactImpulse.x + contactImpulse.z*contactImpulse.z);
	if(planarImpulse > -contactImpulse.y * staticFriction)
	{
		contactImpulse.x /= planarImpulse;
		contactImpulse.z /= planarImpulse;

		contactImpulse.y = (deltaVelocity(0,1) * dynamicFriction * contactImpulse.x) + 
					deltaVelocity(1,1) +
					(deltaVelocity(2,1) * dynamicFriction * contactImpulse.z);
		contactImpulse.y = desiredDeltaVel / contactImpulse.y;
		contactImpulse.x *= dynamicFriction * contactImpulse.y;
		contactImpulse.z *= dynamicFriction * contactImpulse.y;
	}
	else
	{
		contactImpulse.x *= -1.0f;
		contactImpulse.z *= -1.0f;
	}

	return contactImpulse;
}

void Contact::MatchAwakeState()
{
	// Collision with world will never cause a RigidBody to wake up
	if(b2 == nullptr) return;

	bool b1Awake = b1->GetAwake();

	// Wake up the sleeping one
	if(b1Awake ^ b2->GetAwake())
	{
		if(b1Awake)	b2->SetAwake();
		else		b1->SetAwake();
	}
}

void Contact::ReverseContact()
{
	normal *= gFloat(-1.0f);
	RigidBody* temp = b1;
	b1 = b2;
	b2 = temp;
}