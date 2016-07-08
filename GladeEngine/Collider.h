#pragma once
#ifndef GLADE_COLLIDER_H
#define GLADE_COLLIDER_H

#include "PhysicMaterial.h"
#ifndef GLADE_VECTOR_H
#include "Math\Vector.h"
#endif
#ifndef GLADE_MATRIX_H
#include "Math\Matrix.h"
#endif
#include "Math\AABB.h"
#include <vector>
#include <set>

#define TRACK_MASS	1	// Should an Collider's/Object's mass be kept/tracked, or only its inverse mass?

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
	enum class ColliderShape { SPHERE=0, BOX=1, CAPSULE=2, CONE=3, CYLINDER=4, PLANE=5, MESH=6 };

	Collider(RigidBody* rb, ColliderShape cs, PhysicMaterial* m, gFloat iMass, Matrix off, int mask) : attachedBody(rb), shape(cs), physicMaterial(m), inverseMass(iMass), offset(off), collisionMask(mask), collisionType(1), enabled(true) 
#ifdef TRACK_MASS
																					, mass(iMass == 0 ? G_MAX : ((gFloat)1.0) / iMass)
#endif
	{ }

	// Calculate complete transformation matrix for this Collider and pre-computer & save any derived geometric data
	virtual void CalcTransformAndDerivedGeometricData(Matrix attachedParentTransform) { transform = attachedParentTransform * offset; position = Vector(transform(3,0), transform(3,1), transform(3,2)); }

	// Return point on Collider farthest in direction 'd'
	// Support function for GJK Collision Detection algorithm
	virtual Vector GetSupportPoint(const Vector& d) = 0;
	
	// Since Collider's will not be at the origin with no rotation, GetSupportPoint needs to account for the Collider's Affine Transformation
	// Given an Affine Transformation defined as "T(x) = Rx + c" with 'R' being a rotation matrix and 'c' being a translation vector,
	// and the function to calculate a support point 'S()' The new equation for getting the support point of a transformed collider is:
	//				"GetSupportPointTransform(d)  = T(S(R^t * d))"
	// This function returns R^t * d so GetSupportPoint can perform T(S(CalcTransformedDirectionVector(d)))
	Vector CalcTransformedDirectionVector(const Vector& d) { return transform.Transpose3Times(d); }

	// Return ColliderShape of this Collider
	ColliderShape GetShape() const { return shape; }

	// Return Mass of this Collider
	gFloat GetMass() const 
	{
#ifdef TRACK_MASS
		return mass;
#else
		return (gFloat)1.0f / iMass;
#endif
	}

	// Return Inverse Mass of this Collider
	gFloat GetInverseMass() const { return inverseMass; }

	// Return Offset Matrix of this Collider
	Matrix GetOffset() const { return offset; }

	// Return Transform Matrix of this Collider
	Matrix GetTransform() const { return transform; }

	// Return Inertia Tensor of this Collider
	Matrix GetInertiaTensor() const { return inertiaTensor; }

	// Return Position/Centroid of this Collider
	Vector GetPosition() const { return position; }

	// Check Collision Mask for specific collision group(s)
	bool QueryCollisionMask(int m) const { return collisionMask & m; }

	// Add collision group(s) to Collision Mask
	void AddCollisionMask(int m) { collisionMask |= m; }

	// Remove collision group(s) from Collision Mask
	void RemoveCollisionMask(int m) { collisionMask &= ~m; }

	// Add collision group(s) to Collision Type
	void AddCollisionType(int t) { collisionType |= t; }

	// Remove collision groups(s) from Collision Type
	void RemoveCollisionType(int t) { collisionType &= ~t; }

	// Return Collision Type of Collider
	int GetCollisionType() { return collisionType; }

	// Enable/Disable Collider
	void Enable() { enabled = true; }
	void Disable() { enabled = false; }
	bool IsEnabled() const { return enabled; }

	// Get AABB that tightly contains this Collider
	AABB GetBounds() { return bounds; }

//  Vector ClosestPointOnBounds(Vector p);	// return closest point on bounds to p

	friend class CollisionTests;

protected:
	ColliderShape	shape;
	PhysicMaterial*	physicMaterial;
	gFloat			inverseMass;
#ifdef TRACK_MASS
	gFloat			mass;
#endif
	Matrix			offset;		// any potential Scaling, Rotational, or Translational offset from its attached RigidBody
	Matrix			transform;
	Matrix			inertiaTensor;
	Vector			position;
	int				collisionMask;	// Defines what the Collider can be hit by.
	int				collisionType;	// Defines what the Collider "is." 
									//	Can collide with other Collider's who's Collision Mask matches this

	bool			enabled;
//	bool			isTrigger;
	AABB			bounds;
	RigidBody*		attachedBody;
};

class SphereCollider : public Collider
{
public:
	SphereCollider(RigidBody* rb, PhysicMaterial* m, gFloat iMass, gFloat rad, Matrix offset=Matrix(), int mask=1) : Collider(rb, ColliderShape::SPHERE, m, iMass, offset, mask), radius(rad)
	{
		inertiaTensor = Matrix::SphereInertiaTensor((gFloat)1.0f / iMass, rad);
	}
	friend class CollisionTests;

	void CalcTransformAndDerivedGeometricData(Matrix attachedParentTransform)
	{
		Collider::CalcTransformAndDerivedGeometricData(attachedParentTransform);

		// No Derived Geometric Data to calculate

		// Calculate AABB
		Vector radiusVec(radius, radius, radius);
		bounds.maximum = position + radiusVec;
		bounds.minimum = position - radiusVec;
	}

	Vector GetSupportPoint(const Vector& d)
	{
		return (d * radius) + position;
	}

protected:
	gFloat radius;
};

class BoxCollider : public Collider
{
public:
	BoxCollider(RigidBody* rb, PhysicMaterial* m, gFloat iMass, Vector halfW, Matrix offset=Matrix(), int mask=1) : Collider(rb, ColliderShape::BOX, m, iMass, offset, mask), halfWidths(halfW)
	{
		if(iMass == gFloat(0.0f))
			inertiaTensor = Matrix::INFINITE_MASS_INERTIA_TENSOR;
		else
			inertiaTensor = Matrix::CuboidInertiaTensor((gFloat)1.0f / iMass, halfW*(gFloat)2.0f);
	}
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
		bounds.maximum = vertices[0]; bounds.minimum = vertices[7];
		for(unsigned int i = 0; i < 8; ++i)
		{
			if(vertices[i].x > bounds.maximum.x) bounds.maximum.x = vertices[i].x;
			else if(vertices[i].x < bounds.minimum.x) bounds.minimum.x = vertices[i].x;
			if(vertices[i].y > bounds.maximum.y) bounds.maximum.y = vertices[i].y;
			else if(vertices[i].y < bounds.minimum.y) bounds.minimum.y = vertices[i].y;
			if(vertices[i].z > bounds.maximum.z) bounds.maximum.z = vertices[i].z;
			else if(vertices[i].z < bounds.minimum.z) bounds.minimum.z = vertices[i].z;
		}
	}


	Vector GetSupportPoint(const Vector& d)
	{
		Vector dir = CalcTransformedDirectionVector(d);
		return Vector(Sign(dir.x) * halfWidths.x, 
						Sign(dir.y) * halfWidths.y, 
						Sign(dir.z) * halfWidths.z) * transform;
	}
protected:
	Vector halfWidths;	// halfwidths of box along the x-, y-, and z- axes

	// Derived data
	Vector u[3];
};

class CapsuleCollider : public Collider
{
public:
	CapsuleCollider(RigidBody* rb, PhysicMaterial* m, gFloat iMass, gFloat rad, gFloat h, Matrix offset=Matrix(), int mask=1) : Collider(rb, ColliderShape::CAPSULE, m, iMass, offset, mask), radius(rad), height(h)
	{
		inertiaTensor = Matrix::CapsuleInertiaTensor((gFloat)1.0f / iMass, h, rad);
	}
	friend class CollisionTests;

	void CalcTransformAndDerivedGeometricData(Matrix attachedParentTransform)
	{
		Collider::CalcTransformAndDerivedGeometricData(attachedParentTransform);

		// Calc 'p' and 'q' - center of top and bottom of inner cylinder and center of Sphere caps
		p = position + transform.Times3(Vector(0.0f, -height/2.0f, 0.0f));
		q = position + transform.Times3(Vector(0.0f, height/2.0f, 0.0f));
		axis = (q-p).Normalized();

		// Calc AABB (actually the the AABB of both Sphere caps)
		Vector radiusVec(radius, radius, radius);
		AABB b1(p - radiusVec, p + radiusVec),
			b2(q - radiusVec, q + radiusVec);

		// Combine into one AABB
		bounds = b1;
		if(b2.maximum.x > bounds.maximum.x) bounds.maximum.x = b2.maximum.x;
		else /*if(b2.min.x < bounds.min.x)*/ bounds.minimum.x = b2.minimum.x;
		if(b2.maximum.y > bounds.maximum.y) bounds.maximum.y = b2.maximum.y;
		else /*if(b2.min.y < bounds.min.y)*/ bounds.minimum.y = b2.minimum.y;
		if(b2.maximum.z > bounds.maximum.z) bounds.maximum.z = b2.maximum.z;
		else /*if(b2.min.z < bounds.min.z)*/ bounds.minimum.z = b2.minimum.z;
		bounds.CalcCenter();
	}

	Vector GetSupportPoint(const Vector& d)
	{
		Vector dir = CalcTransformedDirectionVector(d);
		Vector r = dir * radius;
		if(dir.y > gFloat(0.0f))
			return r + p;
		if(dir.y < gFloat(0.0f))
			return r + q;
		return r + position;
	}
protected:
	gFloat radius;		// radius of 2 half-sphere's on ends of capsule
	gFloat height;		// height of cylinder portion

	// Derived Data
	Vector p, q;
	Vector axis;

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
	CylinderCollider(RigidBody* rb, PhysicMaterial* m, gFloat iMass, gFloat rad, gFloat h, Matrix offset=Matrix(), int mask=1) : Collider(rb, ColliderShape::CYLINDER, m, iMass, offset, mask), radius(rad), height(h)
	{
		inertiaTensor = Matrix::CylinderInertiaTensor((gFloat)1.0f / iMass, h, rad);
	}
	friend class CollisionTests;

	void CalcTransformAndDerivedGeometricData(Matrix attachedParentTransform)
	{
		Collider::CalcTransformAndDerivedGeometricData(attachedParentTransform);

		// Calc center of top and bottom of Cylinder Collider, and the rotational axis that runs between them
		p = position + transform.Times3(Vector(0.0f, -height/2.0f, 0.0f));
		q = position + transform.Times3(Vector(0.0f, height/2.0f, 0.0f));
		axis = (q-p).Normalized();

		// Calculate AABB: http://www.gamedev.net/topic/338522-bounding-box-for-a-cylinder/
		bounds.minimum = Vector::VectorMin(p, q);
		bounds.maximum = Vector::VectorMax(p, q);

		// Image I drew to prove/check the math: http://i.imgur.com/tkGx4eM.png
		gFloat diffX = p.x-q.x, diffY = p.y-q.y, diffZ = p.z-q.z;
		diffX *= diffX; diffY *= diffY; diffZ *= diffZ;
		gFloat kx = Sqrt(diffY + diffZ) / height; //Sqrt((diffY + diffZ) / (diffX + diffY + diffZ));
		gFloat ky = Sqrt(diffX + diffZ) / height; //Sqrt((diffX + diffZ) / (diffX + diffY + diffZ));
		gFloat kz = Sqrt(diffX + diffY) / height; //Sqrt((diffX + diffY) / (diffX + diffY + diffZ));

		Vector v(kx * radius, ky * radius, kz * radius);
		bounds.minimum -= v;
		bounds.maximum += v;
		bounds.CalcCenter();

	}

	Vector GetSupportPoint(const Vector& d)
	{
		Vector dir = CalcTransformedDirectionVector(d);
		gFloat w = Sqrt(dir.x*dir.x + dir.z*dir.z);

		if(w > 0)
		{
			gFloat a = radius / w;
			return Vector(a*dir.x, 
						(dir.y>0 ? gFloat(1.0f) : gFloat(-1.0f)) * height*gFloat(0.5f), 
						a*dir.z) * transform;
		}

		if(dir.y > gFloat(0.0f))	return p;
		else						return q;
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

  Tip = (center + (0,1,0)*height/2) * transform
  Axis = (0, -1, 0)
*/
class ConeCollider : public Collider
{
public:
	ConeCollider(RigidBody* rb, PhysicMaterial* m, gFloat iMass, gFloat rad, gFloat h, Matrix offset=Matrix(), int mask=1) : Collider(rb, ColliderShape::CONE, m, iMass, offset, mask), radius(rad), height(h), theta(ATan(radius/height))
	{
		inertiaTensor = Matrix::ConeInertiaTensor((gFloat)1.0f / iMass, h, rad);
	}
	friend class CollisionTests;

	void CalcTransformAndDerivedGeometricData(Matrix attachedParentTransform)
	{
		Collider::CalcTransformAndDerivedGeometricData(attachedParentTransform);

		// Calc Tip of cone and it's rotational axis (from tip to center of base)
		tip = position + transform.Times3(Vector(0.0f, height * 0.5f, 0.0f));
		axis = (position - tip).Normalized();

		// Calculate AABB (same AABB as a Cylinder)
		Vector base = tip + (axis * height);
		bounds.minimum = Vector::VectorMin(base, tip);
		bounds.maximum = Vector::VectorMax(tip, base);

		gFloat diffX = tip.x-base.x, diffY = tip.y-base.y, diffZ = tip.z-base.z;
		diffX *= diffX; diffY *= diffY; diffZ *= diffZ;
		gFloat kx = Sqrt(diffX + diffZ) / height;	// Sqrt((diffY + diffZ) / (diffX + diffY + diffZ));
		gFloat ky = Sqrt(diffZ + diffZ) / height;	// Sqrt((diffX + diffZ) / (diffX + diffY + diffZ));
		gFloat kz = Sqrt(diffX + diffY) / height;	// Sqrt((diffX + diffY) / (diffX + diffY + diffZ));

		Vector v(kx * radius, ky * radius, kz * radius);
		bounds.minimum -= v;
		bounds.maximum += v;
		bounds.CalcCenter();
	}

	Vector GetSupportPoint(const Vector& d)
	{
		Vector dir = CalcTransformedDirectionVector(d);
		if(tip.y > dir * Sin(theta))
			return tip;
		
		gFloat w = Sqrt(dir.x*dir.x + dir.z * dir.z);
		if(w > gFloat(0.0f))
		{
			gFloat a = radius / w;
			return Vector(a * dir.x, -height * gFloat(0.5f), a * dir.z) * transform;
		}

		return tip + axis*height;
	}
protected:
	gFloat radius;
	gFloat height;

	// Derived data
	gFloat theta;	
	Vector tip;
	Vector axis;	// Normalized vector from tip to base
};

class PlaneCollider : public Collider
{
public:
	// Width and Height - assume the plane is the floor with its normal pointing straight up the Y-axis. Width goes down the X-axis and height the Z-axis
		// If the plane is a wall with the normal pointing straight at you, Width goes sideways, height goes up/down
	PlaneCollider(RigidBody* rb, PhysicMaterial* m, gFloat iMass, Vector n, gFloat _d, gFloat w, gFloat h, int mask=1) : Collider(rb, ColliderShape::PLANE, m, iMass, Matrix(), mask), normal(n), d(_d), width(w), height(h) { }
	friend class CollisionTests;

	// Plane collider unique. It never updates after initial creation, because anything that has a Plane collider shound not move...ever!
	// ATTEMPING TO UPDATE/MOVE/ROTATE IT VIA THIS FUNCTION WILL NOT WORK.
	// A plane cannot have an AABB, so one is never calculated.
	void CalcTransformAndDerivedGeometricData(Matrix attachedParentTransform) { return;	}

	Vector GetSupportPoint(const Vector& d) { assert(false); return Vector(); }

protected:
	Vector normal;	// plane normal
	gFloat d;		// distance from origin to plane
	gFloat width;
	gFloat height;
};

class MeshCollider: public Collider
{
public:
	MeshCollider(RigidBody* rb, PhysicMaterial* m, gFloat iMass, std::vector<Vector> verts, int mask=1) : Collider(rb, ColliderShape::MESH, m, iMass, Matrix(), mask) 
	{
		for(auto iter = verts.begin(); iter != verts.end(); ++iter)
			vertices.insert(*iter);
	}
	friend class CollisionTests;

	Vector GetSupportPoint(const Vector& d) 
	{ 
		Vector dir = CalcTransformedDirectionVector(d);
		gFloat dist = G_MIN, dot;
		Vector p;
		for(auto iter = vertices.begin(); iter != vertices.end(); ++iter)
		{
			dot = dir.DotProduct(*iter);
			if(dot > dist)
			{
				dist = dot;
				p = *iter;
			}
		}
		return p * transform; 
	}

protected:
	std::set<Vector> vertices;
};

} // namespace Glade
#endif //GLADE_COLLIDER_H

