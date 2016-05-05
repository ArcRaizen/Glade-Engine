#pragma once
#ifndef GLADE_OBJECT_H
#define GLADE_OBJECT_H

#include "GladeConfig.h"
#include "Math\Vector.h"
#include "System\Graphics\GraphicsLocator.h"
#include "System\GeometryGenerator.h"

#ifndef GLADE_COLLIDER_H
#include "Collider.h"
#endif
#include <vector>
#include <set>

#define OVERRIDE_POSITION		(1 << 0)	// Able to have its position overwritten
#define OVERRIDE_CENTROID		(1 << 1)	// Able to have its centroid overwritten
#define	OVERRIDE_VELOCITY		(1 << 2)	// Able to have its velocity overwritten
#define OVERRIDE_ACCELERATION	(1 << 3)	// Able to have its acceleration overwritten

namespace Glade {
class Object
{
public:
	Object();
	Object(Vector p, Vector a, gFloat lDamp, bool ug, Vector grav=Vector());

	// NOTE! This Constructor should only be used by Particles
	Object(Vector p, Vector a, gFloat iMass, gFloat lDamp, bool ug, Vector grav=Vector());
	virtual ~Object() { }

	virtual bool Update() = 0;				// Do physics
	virtual void Render() = 0;
	virtual void CalcBoundingBox() = 0;		// Calculate tight Axis-Aligned Bounding Box that wholly contains this Object

	// Basic Comparison
	bool	operator== (const Object& other);
	bool	operator< (const Object& other);
	unsigned int	GetID();

	// Apply a new force to this Object.
	// Accumulate the new force onto all other forces already applied this frame
	void ApplyForce(const Vector& f);

	// Set awake status of Object
	virtual void SetAwake(bool awake=true) = 0;
	
	// Check if Object is current awake or asleep
	virtual bool GetAwake() const;

	// Set whether this Object can be be put to sleep
	virtual void SetCanSleep(bool sleep=true);

	// Check whether this Object can be put to sleep
	virtual bool GetCanSleep() const;

	// Change if/how this Object experience Gravity
	void TurnGravityOn(Vector grav=Vector::GRAVITY);
	void TurnGravityOff();

	// Keep ID of ForceGenerator exerting force on this Particle
	void RegisterForceGenerator(int id);

	// ForceGenerator no longer acting on this Particle, remove its ID
	void UnregisterForceGenerator(int id);

	std::vector<int> GetRegisteredForceGenerators();

	// Maintain accessabiltiy of Object's physical properties
	void AllowSetPosition();
	void DisallowSetPosition();
	bool CheckAllowSetPosition();
	void AllowSetCentroid();
	void DisallowSetCentroid();
	bool CheckAllowSetCentroid();
	void AllowSetVelocity();
	void DisallowSetVelocity();
	bool CheckAllowSetVelocity();
	void AllowSetAcceleration();
	void DisallowSetAcceleration();
	bool CheckAllowSetAcceleration();

	// Access/Mutate Object's physical properties
	Vector GetPosition() const;
	void SetPosition(const Vector& p);
	void SetPosition(const gFloat x, const gFloat y, const gFloat z);
	Vector GetCentroid() const;
	void SetCentroid(const Vector& c);
	void SetCentroid(const gFloat x, const gFloat y, const gFloat z);
	virtual Vector GetVelocity() const = 0;
	virtual void SetVelocity(const Vector& v) = 0;
	virtual void SetVelocity(const gFloat x, const gFloat y, const gFloat z) = 0;
	Vector GetAcceleration() const;
	void SetAcceleration(const Vector& a);
	void SetAcceleration(const gFloat x, const gFloat y, const gFloat z);

	// Return Mass of Object
	gFloat GetMass() const;

	// Return Inverse Mass of Object
	gFloat GetInverseMass() const;

	Matrix GetTransformMatrix() const;
	void GetTransformMatrix(Matrix* m) const;
	virtual Matrix GetInverseInertiaTensorWorld() const = 0;
	virtual void GetInverseInertiaTensorWorld(Matrix* m) const = 0;

	// Calculate and Return AABB that contains this Object
	AABB GetBoundingBox();

	// Return the radius of a Sphere wholly encapsulating this Object
	gFloat GetRadius();

	// Return the radius squared of a Sphere wholly encapsulating this Object
	gFloat GetRadiusSquared();

	std::set<int> GetHashIndices();
	void SetHashIndices(std::set<int> indices);

protected:
	struct Identification
	{
		static unsigned int ID() { static unsigned int id = 0; return id++; }
	};
	unsigned int ID;


	Vector	position;		// Reference point in World Space of Object
	Vector	centroid;		// While position is a reference point in world space visually
								//	the centroid is the center of mass of the RigidBody
	Vector	velocity;		// Linear Velocity of Object
	Vector	acceleration;	// Linear Acceleration of Object this frame
	Vector	lastAcceleration;// Linear Acceleration of Object last frame
	bool	isAwake;		// Object can be put to sleep to avoid being updated by integration function or contact resolution
	bool	canSleep;		// Is this Object permitted to be put to sleep (bad idea for user-controlled Objects)

	gFloat	linearDamping;	// Damping/drag appiled to linear motion

#ifdef TRACK_MASS
	gFloat	mass;			// Mass of the Object
#endif
	gFloat	inverseMass;	// Inverse Mass (1/mass) of the Object (0 means infinite mass)

	Matrix	transformationMatrix;
	Matrix	inverseInertiaTensor;

	bool	useGravity;		// Should this Object experience Gravity
	Vector	gravity;		// Gravitational acceleration experience by this Object

	Vector	force;			// Accumulation of all force acting on Object this frame

	gFloat	radius;
	AABB	boundingBox;
	bool	recalcAABB;

	std::set<int>			hashIndices;

	std::vector<int> generatorIDs;

	Direct3D::ShaderResource* shaderResource;

	int		properties;
};
}	// namespace
#endif // GLADE_OBJECT_H