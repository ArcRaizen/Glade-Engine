#pragma once
#ifndef GLADE_OBJECT_H
#define GLADE_OBJECT_H

#ifndef GLADE_VECTOR_H
#include "Math\Vector.h"
#endif
#ifndef GLADE_CORE_H
#include "Core.h"
#endif
#ifndef GLADE_COLLIDER_H
#include "Collider.h"
#endif
#include <vector>

#define TRACK_MASS				1			// Should an Object's mass be kept, or only its inverse mass?
#define OVERRIDE_POSITION		(1 << 0)	// Able to have its position overwritten
#define	OVERRIDE_VELOCITY		(1 << 1)	// Able to have its velocity overwritten
#define OVERRIDE_ACCELERATION	(1 << 2)	// Able to have its acceleration overwritten

namespace Glade {
class Object
{
public:
	Object() : position(Vector()), acceleration(Vector()), inverseMass(1), isAwake(true), canSleep(true), useGravity(true), gravity(Vector::GRAVITY), properties(0), ID(Identification::ID())
#ifdef TRACK_MASS
				, mass(1)
#endif
	{ }
	Object(Vector p, Vector a, gFloat iMass, bool ug, Vector grav=Vector()) : position(p), acceleration(a), inverseMass(iMass), isAwake(iMass>0?true:false), canSleep(true),  useGravity(ug), gravity(grav), properties(0), ID(Identification::ID())
#ifdef TRACK_MASS
																				, mass(iMass == 0 ? G_MAX : ((gFloat)1.0) / iMass)
#endif
	{ }
	virtual ~Object() { }

	virtual bool Update() = 0;				// Do physics
	virtual void CalcBoundingBox() = 0;		// Calculate tight Axis-Aligned Bounding Box that wholly contains this Object

	bool	operator== (const Object& other) { return ID == other.ID; }
	bool	operator< (const Object& other) { return ID < other.ID; }
	unsigned int	GetID() { return ID; }

	// Apply a new force to this Object.
	// Accumulate the new force onto all other forces already applied this frame
	void ApplyForce(const Vector& f)
	{
		// Do not apply force to infinitely-massed Object
		if(inverseMass == 0) return;
		force += f;
		isAwake = true;
	}

	// Set awake status of Object
	virtual void SetAwake(bool awake=true) = 0;
	
	// Check if Object is current awake or asleep
	virtual bool GetAwake() const { return isAwake; }

	// Set whether this Object can be be put to sleep
	virtual void SetCanSleep(bool sleep=true)
	{
		canSleep = sleep;
		if(!canSleep && !isAwake)
			SetAwake();
	}

	// Check whether this Object can be put to sleep
	virtual bool GetCanSleep() const { return canSleep; }

	// Change if/how this Object experience Gravity
	void TurnGravityOn(Vector grav=Vector::GRAVITY) { useGravity = true; gravity = grav; }
	void TurnGravityOff() { useGravity = false; }

	// Maintain accessabiltiy of Object's physical properties
	void AllowSetPosition() { properties |= OVERRIDE_POSITION; }
	void DisallowSetPosition() { properties &= ~OVERRIDE_POSITION; }
	bool CheckAllowSetPosition() { return properties & OVERRIDE_POSITION; }
	void AllowSetAcceleration() { properties |= OVERRIDE_ACCELERATION; }
	void DisallowSetAcceleration() { properties &= ~OVERRIDE_ACCELERATION; }
	bool CheckAllowSetAcceleration() { return properties & OVERRIDE_ACCELERATION; }

	// Access/Mutate Object's physical properties
	Vector GetPosition() const { return position; }
	void SetPosition(const Vector& p) { if(!(properties & OVERRIDE_POSITION)) return;  position = p; }
	void SetPosition(const gFloat x, const gFloat y, const gFloat z)
	{
		if(!(properties & OVERRIDE_POSITION)) return;
		position.x = x;
		position.y = y;
		position.z = z;
	}
	Vector GetAcceleration() const { return acceleration; }
	void SetAcceleration(const Vector& a) { if(!(properties & OVERRIDE_ACCELERATION)) return; acceleration = a; }
	void SetAcceleration(const gFloat x, const gFloat y, const gFloat z)
	{
		if(!(properties & OVERRIDE_ACCELERATION)) return;
		acceleration.x = x;
		acceleration.y = y;
		acceleration.z = z;
	}

	// Return Mass of Object
	gFloat GetMass() const
	{
#ifdef TRACK_MASS
		return mass;
#endif
		return inverseMass == 0 ? G_MAX : ((gFloat)1.0 / inverseMass);
	}

	// Return Inverse Mass of Object
	gFloat GetInverseMass() const { return inverseMass; }

	// Set Mass of Object
	void SetMass(gFloat m)
	{
		assert(mass > 0);
#ifdef TRACK_MASS
		mass = m;
#endif
		inverseMass = ((gFloat)1.0) / m;
	}

	// Set Inverse Mass of Object
	void SetInverseMass(gFloat im)
	{
		inverseMass = im;
#ifdef TRACK_MASS
		mass = im == 0 ? G_MAX : ((gFloat)1.0) / im;
#endif
	}

	// Calculate and Return AABB that contains this Object
	AABB GetBoundingBox() { CalcBoundingBox(); return boundingBox; }

	// Return the radius of a Sphere wholly encapsulating this Object
	gFloat GetRadius() { return radius; }

	// Return the radius squared of a Sphere wholly encapsulating this Object
	gFloat GetRadiusSquared() { return radius * radius; }

	void AddCollider(Collider* collider) { colliders.push_back(collider); CalcBoundingBox(); }

	unsigned int GetColliders(std::vector<Collider*>& c) { c = colliders; return colliders.size(); }

	std::vector<int> GetHashIndices() { return hashIndices; }
	void SetHashIndices(std::vector<int> indices) { hashIndices = indices; }

protected:
	struct Identification
	{
		static unsigned int ID() { static unsigned int id = 0; return id++; }
	};
	unsigned int ID;

	Vector	position;		// Position of Object in world space
	Vector	acceleration;	// Linear Acceleration of Object this frame
	Vector	lastAcceleration;// Linear Acceleration of Object last frame
	bool	isAwake;		// Object can be put to sleep to avoid being updated by integration function or contact resolution
	bool	canSleep;		// Is this Object permitted to be put to sleep (bad idea for user-controlled Objects)

#ifdef TRACK_MASS
	gFloat	mass;			// Mass of the Object
#endif
	gFloat	inverseMass;	// Inverse Mass (1/mass) of the Object (0 means infinite mass)

	bool	useGravity;		// Should this Object experience Gravity
	Vector	gravity;		// Gravitational acceleration experience by this Object

	Vector	force;			// Accumulation of all force acting on Object this frame


	gFloat	radius;
	AABB	boundingBox;
	bool	recalcAABB;
	std::vector<Collider*> colliders;
	std::vector<int>	hashIndices;

	int		properties;
};
}	// namespace
#endif // GLADE_OBJECT_H