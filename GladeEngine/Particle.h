#pragma once
#ifndef GLADE_PARTICLE_H
#define GLADE_PARTICLE_H

#ifndef GLADE_OBJECT_H
#include "Object.h"
#endif
#include "Sleep.h"
#include <vector>

namespace Glade {
class ParticleContact;
class Particle : public Object
{
public:
	Particle();
	Particle(Vector pos, Vector vel, Vector accel, gFloat damp, gFloat rad, gFloat iMass, bool ug, Vector grav=Vector());

	virtual ~Particle();

	bool Update();				// Perform physics integration and update dynamic state properties
	void Render();
	void CalcBoundingBox();		// Calculate and save the AABB that contains this Particle

	void SetAwake(bool awake=true);

	// Properties
	Vector	GetVelocity() const;
	void	SetVelocity(const Vector& v);
	void	SetVelocity(const gFloat x, const gFloat y, const gFloat z);

	Matrix GetInverseInertiaTensorWorld() const;
	void GetInverseInertiaTensorWorld(Matrix* m) const;

private:
	friend class ParticleContact;
	void	ForceSetPosition(const Vector& p);
	void	ForceSetVelocity(const Vector& v);
	void	ForceSetAcceleration(const Vector& a);

protected:
#ifdef VERLET
	Vector  prevPosition;		// Position of Particle last frame
#else
	bool	velocityModified;
#endif

	gFloat	motion;				// The amount of motion of the Object.
	gFloat	motionBias;			// Ratio at which an Object's motion this frame is favored compared to previous frames when calculating 
									// average motion over recent frames

	static unsigned int PARTICLE_MESH_STACKS;
	static unsigned int PARTICLE_MESH_SLICES;
	static D3DXVECTOR4 PARTICLE_MESH_COLOR;
};
}	// namespace
#endif	// GLADE_PARTICLE_H
