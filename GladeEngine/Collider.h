#pragma once
#ifndef GLADE_COLLIDER_H
#define GLADE_COLLIDER_H

#ifndef GLADE_VECTOR_H
#include "Math\Vector.h"
#endif
#ifndef GLADE_CORE_H
#include "Core.h"
#endif
#include <vector>

namespace Glade {
/*
	Collider's are attached to RigidBody's to allow for simplified collision detection
	and Contact generation utilizing a set of pre-defined shape primities. 

	Collider's may also be attached to other objects to utilize any necessary collision
	detection without Contact generation
	
	The PLANE primitive shape is not used for RigidBodys, but useful for collision with world
	geometry and during some intermediate collision-detection algorithms. It is included here
	for simplicity.
*/
class RigidBody;
class CollisionTests;
class Collider
{
public:
	enum class ColliderShape { SPHERE=0, BOX=1, CYLINDER=2, CONE=3, CAPSULE=4, PLANE=5, MESH=6 };

	Collider(RigidBody* rb, ColliderShape cs, Matrix off, int mask) : attachedBody(rb), shape(cs), offset(off), collisionMask(mask), enabled(true) { }

	// Calculate complete transformation matrix for this Collider and pre-computer & save any derived geometric data
	virtual void CalcTransformAndDerivedGeometricData(Matrix attachedParentTransform) { transform = attachedParentTransform * offset; position = Vector(transform(3,0), transform(3,1), transform(3,2)); }

	// Return ColliderShape of this Collider
	ColliderShape GetShape() const { return shape; }

	// Return Offset Matrix of this Collider
	Matrix GetOffset() const { return offset; }

	// Return Transform Matrix of this Collider
	Matrix GetTransform() const { return transform; }

	// Check Collision Mask for specific collision group(s)
	bool QueryCollisionMask(int m) const { return collisionMask & m; }

	// Add collision group(s) to Collision Mask
	void AddCollisionMask(int m) { collisionMask |= m; }

	// Remove collision group(s) from Collision Mask
	void RemoveCollisionMask(int m) { collisionMask &= ~m; }

	// Enable/Disable Collider
	void Enable() { enabled = true; }
	void Disable() { enabled = false; }
	bool IsEnabled() const { return enabled; }

	// Get AABB that tightly contains this Collider
	AABB GetBounds() { return bounds; }

//  Vector ClosestPointOnBounds(Vector p);	// return closest point on bounds to p
//  bool Raycast();							// test raycast against collider

	friend class CollisionTests;

protected:
	ColliderShape	shape;
	Matrix			offset;		// any potential Scaling, Rotational, or Translational offset from its attached RigidBody
	Matrix			transform;
	Vector			position;
	int				collisionMask;

	bool			enabled;
//	bool			isTrigger;
	AABB			bounds;
	RigidBody*		attachedBody;
};

class SphereCollider : public Collider
{
public:
	SphereCollider(RigidBody* rb, gFloat rad, Matrix offset=Matrix(), int mask=1) : Collider(rb, ColliderShape::SPHERE, offset, mask), radius(rad) { }
	friend class CollisionTests;

	void CalcTransformAndDerivedGeometricData(Matrix attachedParentTransform)
	{
		Collider::CalcTransformAndDerivedGeometricData(attachedParentTransform);

		// No Derived Geometric Data to calculate

		// Calculate AABB
		Vector radiusVec(radius, radius, radius);
		bounds.max = position + radiusVec;
		bounds.min = position - radiusVec;
	}

protected:
	gFloat radius;
};

class BoxCollider : public Collider
{
public:
	BoxCollider(RigidBody* rb, Vector halfW, Matrix offset=Matrix(), int mask=1) : Collider(rb, ColliderShape::BOX, offset, mask), halfWidths(halfW) { }
	friend class CollisionTests;

	void CalcTransformAndDerivedGeometricData(Matrix attachedParentTransform)
	{
		Collider::CalcTransformAndDerivedGeometricData(attachedParentTransform);

		// Calc local x-, y-, z- axes of Box Collider
		u[0] = Vector(transform(0,0), transform(0,1), transform(0,2));
		u[1] = Vector(transform(1,0), transform(1,1), transform(1,2));
		u[2] = Vector(transform(2,0), transform(2,1), transform(2,2));
		if(Abs(u[0].SquaredMagnitude() - 1) > EPSILON) u[0].NormalizeInPlace();
		if(Abs(u[1].SquaredMagnitude() - 1) > EPSILON) u[1].NormalizeInPlace();
		if(Abs(u[2].SquaredMagnitude() - 1) > EPSILON) u[2].NormalizeInPlace();

		// Calculate vertices of OOBB
		Vector vertices[8] = {
			u[0]*halfWidths.x + u[1]*halfWidths.y + u[2]*halfWidths.z + position,
			u[0]*halfWidths.x + u[1]*halfWidths.y + u[2]*-halfWidths.z + position,
			u[0]*halfWidths.x + u[1]*-halfWidths.y + u[2]*halfWidths.z + position,
			u[0]*halfWidths.x + u[1]*-halfWidths.y + u[2]*-halfWidths.z + position,
			u[0]*-halfWidths.x + u[1]*halfWidths.y + u[2]*halfWidths.z + position,
			u[0]*-halfWidths.x + u[1]*halfWidths.y + u[2]*-halfWidths.z + position,
			u[0]*-halfWidths.x + u[1]*-halfWidths.y + u[2]*halfWidths.z + position,
			u[0]*-halfWidths.x + u[1]*-halfWidths.y + u[2]*-halfWidths.z + position,
		};

		// Calc AABB from OOBB vertices
		bounds.max = vertices[0]; bounds.min = vertices[7];
		for(unsigned int i = 0; i < 8; ++i)
		{
			if(vertices[i].x > bounds.max.x) bounds.max.x = vertices[i].x;
			else if(vertices[i].x < bounds.min.x) bounds.min.x = vertices[i].x;
			if(vertices[i].y > bounds.max.y) bounds.max.y = vertices[i].y;
			else if(vertices[i].y < bounds.min.y) bounds.min.y = vertices[i].y;
			if(vertices[i].z > bounds.max.z) bounds.max.z = vertices[i].z;
			else if(vertices[i].z < bounds.min.z) bounds.min.z = vertices[i].z;
		}
	}
protected:
	Vector halfWidths;	// halfwidths of box along the x-, y-, and z- axes

	// Derived data
	Vector u[3];
};

/* Assume cylinder default orientation is:
	______
   |      |
   |      |
   |      |
   |      |
   |______|

   Bottom (P) = (center + (0, -height/2, 0)) * transform
   Top (Q) = (center + (0, height/2, 0)) * transform
*/
class CylinderCollider : public Collider
{
public:
	CylinderCollider(RigidBody* rb, gFloat rad, gFloat h, Matrix offset=Matrix(), int mask=1) : Collider(rb, ColliderShape::CYLINDER, offset, mask), radius(rad), height(h) { }
	friend class CollisionTests;

	void CalcTransformAndDerivedGeometricData(Matrix attachedParentTransform)
	{
		Collider::CalcTransformAndDerivedGeometricData(attachedParentTransform);

		// Calc center of top and bottom of Cylinder Collider, and the rotational axis that runs between them
		p = position + transform.Multiply3(Vector(0.0f, -height/2.0f, 0.0f));
		q = position + transform.Multiply3(Vector(0.0f, height/2.0f, 0.0f));
		axis = (q-p).Normalized();

		// Calculate AABB: http://www.gamedev.net/topic/338522-bounding-box-for-a-cylinder/
		bounds.min = Vector::VectorMin(p, q);
		bounds.max = Vector::VectorMax(p, q);

		// Image I drew to prove/check the math: http://i.imgur.com/tkGx4eM.png
		gFloat diffX = p.x-q.x, diffY = p.y-q.y, diffZ = p.z-q.z;
		diffX *= diffX; diffY *= diffY; diffZ *= diffZ;
		gFloat kx = Sqrt(diffY + diffZ) / height; //Sqrt((diffY + diffZ) / (diffX + diffY + diffZ));
		gFloat ky = Sqrt(diffX + diffZ) / height; //Sqrt((diffX + diffZ) / (diffX + diffY + diffZ));
		gFloat kz = Sqrt(diffX + diffY) / height; //Sqrt((diffX + diffY) / (diffX + diffY + diffZ));

		Vector v(kx * radius, ky * radius, kz * radius);
		bounds.min -= v;
		bounds.max += v;

	}
protected:
	gFloat radius;
	gFloat height;

	// Derived data
	Vector p;		// Bottom
	Vector q;		// Top
	Vector axis;	// Normalized vector from p to q
};

/* Assume cone default orientation is:
	/\
   /  \
  /____\

  Tip = (center + (0,1,0)*height) * transform
  Axis = (0, -1, 0)
*/
class ConeCollider : public Collider
{
public:
	ConeCollider(RigidBody* rb, gFloat rad, gFloat h, Matrix offset=Matrix(), int mask=1) : Collider(rb, ColliderShape::CONE, offset, mask), radius(rad), height(h), theta(ATan(radius/height)) { }
	friend class CollisionTests;

	void CalcTransformAndDerivedGeometricData(Matrix attachedParentTransform)
	{
		Collider::CalcTransformAndDerivedGeometricData(attachedParentTransform);

		// Calc Tip of cone and it's rotational axis (from tip to center of base)
		tip = position + transform.Multiply3(Vector(0.0f, height/2.0f, 0.0f));
		axis = (position - tip).Normalized();

		// Calculate AABB (same AABB as a Cylinder)
		Vector base = tip + (axis * height);
		bounds.min = Vector::VectorMin(base, tip);
		bounds.max = Vector::VectorMax(tip, base);

		gFloat diffX = tip.x-base.x, diffY = tip.y-base.y, diffZ = tip.z-base.z;
		diffX *= diffX; diffY *= diffY; diffZ *= diffZ;
		gFloat kx = Sqrt(diffX + diffZ) / height;	// Sqrt((diffY + diffZ) / (diffX + diffY + diffZ));
		gFloat ky = Sqrt(diffZ + diffZ) / height;	// Sqrt((diffX + diffZ) / (diffX + diffY + diffZ));
		gFloat kz = Sqrt(diffX + diffY) / height;	// Sqrt((diffX + diffY) / (diffX + diffY + diffZ));

		Vector v(kx * radius, ky * radius, kz * radius);
		bounds.min -= v;
		bounds.max += v;
	}
protected:
	gFloat radius;
	gFloat height;

	// Derived data
	gFloat theta;	
	Vector tip;
	Vector axis;	// Normalized vector from tip to base
};


class CapsuleCollider : public Collider
{
public:
	CapsuleCollider(RigidBody* rb, gFloat rad, gFloat h, Matrix offset=Matrix(), int mask=1) : Collider(rb, ColliderShape::CAPSULE, offset, mask), radius(rad), height(h) { }
	friend class CollisionTests;

	void CalcTransformAndDerivedGeometricData(Matrix attachedParentTransform)
	{
		Collider::CalcTransformAndDerivedGeometricData(attachedParentTransform);

		// Calc 'p' and 'q' - center of top and bottom of inner cylinder and center of Sphere caps
		p = position + transform.Multiply3(Vector(0.0f, -height/2.0f, 0.0f));
		q = position + transform.Multiply3(Vector(0.0f, height/2.0f, 0.0f));
		axis = (q-p).Normalized();

		// Calc AABB (actually the the AABB of both Sphere caps)
		AABB b1, b2;
		Vector radiusVec(radius, radius, radius);
		b1.max = p + radiusVec;
		b1.min = p - radiusVec;
		b2.max = q + radiusVec;
		b2.min = q - radiusVec;

		// Combine into one AABB
		bounds = b1;
		if(b2.max.x > bounds.max.x) bounds.max.x = b2.max.x;
		else /*if(b2.min.x < bounds.min.x)*/ bounds.min.x = b2.min.x;
		if(b2.max.y > bounds.max.y) bounds.max.y = b2.max.y;
		else /*if(b2.min.y < bounds.min.y)*/ bounds.min.y = b2.min.y;
		if(b2.max.z > bounds.max.z) bounds.max.z = b2.max.z;
		else /*if(b2.min.z < bounds.min.z)*/ bounds.min.z = b2.min.z;
	}
protected:
	gFloat radius;		// radius of 2 half-sphere's on ends of capsule
	gFloat height;		// height of cylinder portion

	// Derived Data
	Vector p, q;
	Vector axis;

};

class PlaneCollider : public Collider
{
public:
	// Width and Height - assume the plane is the floor with its normal pointing straight up the Y-axis. Width goes down the X-axis and height the Z-axis
		// If the plane is a wall with the normal pointing straight at you, Width goes sideways, height goes up/down
	PlaneCollider(RigidBody* rb, Vector n, gFloat _d, gFloat w, gFloat h, int mask=1) : Collider(rb, ColliderShape::PLANE, Matrix(), mask), normal(n), d(_d), width(w), height(h) { }
	friend class CollisionTests;

	// Plane collider unique. It never updates after initial creation, because anything that has a Plane collider shound not move...ever!
	// ATTEMPING TO UPDATE/MOVE/ROTATE IT VIA THIS FUNCTION WILL NOT WORK.
	// A plane cannot have an AABB, so one is never calculated.
	void CalcTransformAndDerivedGeometricData(Matrix attachedParentTransform) { return;	}

protected:
	Vector normal;	// plane normal
	gFloat d;		// distance from origin to plane
	gFloat width;
	gFloat height;
};

class MeshCollider: public Collider
{
public:
	MeshCollider(RigidBody* rb, int mask=1) : Collider(rb, ColliderShape::MESH, Matrix(), mask) { }
	friend class CollisionTests;
};

} // namespace Glade
#endif //GLADE_COLLIDER_H

