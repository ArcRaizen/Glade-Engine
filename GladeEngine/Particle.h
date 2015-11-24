#pragma once
#ifndef GLADE_PARTICLE_H
#define GLADE_PARTICLE_H

#ifndef GLADE_OBJECT_H
#include "Object.h"
#endif
#include <vector>

namespace Glade {
class ParticleContact;
class Particle : public Object
{
public:
	Particle();
	Particle(Vector pos, Vector vel, Vector accel, gFloat damp, gFloat iMass, gFloat rad, bool ug, Vector grav=Vector());

	virtual ~Particle();

	bool Update();				// Perform physics integration and update dynamic state properties
	void CalcBoundingBox();		// Calculate and save the AABB that contains this Particle

	void SetAwake(bool awake=true);

	void RegisterForceGenerator(int id);			// Keep ID of ForceGenerator exerting force on this Particle
	void UnregisterForceGenerator(int id);			// ForceGenerator no longer acting on this Particle, remove its ID
	std::vector<int> GetRegisteredForceGenerators();

	void AllowSetVelocity();			// Let the particle have its velocity set by SetVelocity
	void DisallowSetVelocity();			// Prevent particle from having its velocity set by SetVelocity (default)
	bool CheckAllowSetVelocity();		// Return if particle may have its velocity set by SetVelocity

	// Properties
	void	GetVelocity(Vector& v) const;
	void	GetVelocity(Vector* v) const;
	Vector	GetVelocity();
	void	SetVelocity(const Vector& v);
	void	SetVelocity(const gFloat x, const gFloat y, const gFloat z);

private:
	friend class ParticleContact;
	void	ForceSetPosition(const Vector& p);
	void	ForceSetVelocity(const Vector& v);
	void	ForceSetAcceleration(const Vector& a);

protected:
	Vector	velocity;			// Linear Velocity of Particle
#ifdef VERLET
	Vector  prevPosition;		// Position of Particle last frame
#else
	bool	velocityModified;
#endif

	gFloat	damping;			// Damping/drag appiled to linear motion

	gFloat	motion;				// The amount of motion of the Object.
	gFloat	motionBias;			// Ratio at which an Object's motion this frame is favored compared to previous frames when calculating 
									// average motion over recent frames

private:
	std::vector<int> generatorIDs;
};
}	// namespace
#endif	// GLADE_PARTICLE_H
