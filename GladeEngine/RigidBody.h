#pragma once
#ifndef GLADE_RIGID_BODY_H
#define GLADE_RIGID_BODY_H

#ifndef GLADE_OBJECT_H
#include "Object.h"
#endif
#include <vector>

#define	OVERRIDE_ANGULAR_VELOCITY		(1 << 3)	// Able to have its angular velocity overwritten
#define OVERRIDE_ANGULAR_ACCELERATION	(1 << 4)	// Able to have its angular acceleration overwritten

namespace Glade {
class Contact;
class RigidBody : public Object
{
public:
	RigidBody();
	RigidBody(Vector pos, Quaternion orient, Vector vel, Vector accel, Vector angVel, Vector angAccel, gFloat lDamp, gFloat aDamp, gFloat iMass, bool ug, Vector grav=Vector());
	virtual ~RigidBody();

	bool Update();				// Perform physics integration and update dynamic state properties
	void CalcDerivedData();		// Calculate internal data from current state.
	void CalcBoundingBox();		// Calculate and save the AABB that wholly contains this Rigid Body

	// Apply a force to this Rigid body...
	void ApplyForceAtPoint(const Vector& f, const Vector& p);		// at another point in world space
	void ApplyForceAtLocalPoint(const Vector& f, const Vector& p);	// at another point in local space 

	void SetAwake(bool awake=true);

	void TurnOnGravity(Vector grav=Vector::GRAVITY);
	void TurnOffGravity();

	void RegisterForceGenerator(int id);			// Keep ID of ForceGenerator exerting force on this Rigid Body
	void UnregisterForceGenerator(int id);			// ForceGenerator no longer acting on this Rigid Body
	std::vector<int> GetRegisteredForceGenerators();

	void AllowSetVelocity();			// Let the Rigid Body have its velocity set by SetVelocity
	void DisallowSetVelocity();			// Prevent Rigid Body from having its velocity set by SetVelocity (default)
	bool CheckAllowSetVelocity();		// Return if Rigid Body may have its velocity set by SetVelocity

	Vector	GetVelocity();
	void	SetVelocity(const Vector& v);
	void	SetVelocity(const gFloat x, const gFloat y, const gFloat z);
	Vector	GetAngularVelocity();
	void	SetAngularVelocity(const Vector& v);
	void	SetAngularVelocity(const gFloat x, const gFloat y, const gFloat z);
	void	GetAngularAcceleration(Vector& a) const;
	void	GetAngularAcceleration(Vector* a) const;
	Vector	GetAngularAcceleration();
	void	SetAngularAcceleration(const Vector& v);
	void	SetAngularAcceleration(const gFloat x, const gFloat y, const gFloat z);

	void	SetInverseInertiaTensor(const Matrix& m);

	Matrix GetTransformMatrix();
	Matrix GetInverseInertiaTensorWorld();

private:
	friend class Contact;
	Vector	GetLastFrameAcceleration() const;
	void	ForceSetPosition(const Vector& p);
	void	ForceSetOrientation(const Quaternion& o);
	void	ForceSetVelocity(const Vector& v);
	void	ForceSetAngularVelocity(const Vector& v);
	void	ForceSetAcceleration(const Vector& a);
	void	ForceSetAngularAcceleration(const Vector& a);
	void	ForceAddPosition(const Vector& p);
	void	ForceAddOrientation(const Vector& o);
	void	ForceAddVelocity(const Vector& v);
	void	ForceAddAngularVelocity(const Vector& v);
	void	ForceAddAcceleration(const Vector& a);
	void	ForceAddAngularAcceleration(const Vector& a);

protected:
	// Static Characteristics
	gFloat	linearDamping;
	gFloat	angularDamping;


	Matrix	inverseInertiaTensor;
	Matrix	inverseInertiaTensorWorld;

	// Dynamic State Properties
	Vector	velocity;
	Vector	angularVelocity;
	bool	velocityModified;
	bool	angularVelocityModified;

	Vector	angularAcceleration;
	Vector	lastAngularAcceleration;

	Matrix	transformationMatrix;

	Quaternion	orientation;

	Vector	torque;

private:
	std::vector<int> generatorIDs;

public:
	inline Vector GetPointInLocalSpace(const Vector& p) const { return p * transformationMatrix.Inverse4(); }
	inline Vector GetPointInWorldSpace(const Vector& p) const { return p * transformationMatrix; } 
};
}	// namespace
#endif // GLADE_RIGID_BODY_H