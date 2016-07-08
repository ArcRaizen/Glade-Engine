#pragma once
#ifndef GLADE_PLANE_S
#define GLADE_PLANE_S

#ifndef GLADE_VECTOR_H
#include "Vector.h"
#endif

namespace Glade {
struct Plane
{
	Plane() { }
	Plane(Vector n, Vector p) : normal(n.Normalized()), point(p), d(normal.DotProduct(point)) {}
	Plane(Vector n, gFloat d_) : normal(n.Normalized()), d(d_) {}
	Plane(Vector p1, Vector p2, Vector p3) : point(p1)
	{
		Vector v = p2 - p1, u = p3 - p1;
		normal = (v * u).Normalized();
		d = -(normal.DotProduct(p1));
	}

	// Set the parameters of this Plane from given normal and a point on the plane
	void SetNormalPoint(Vector n, Vector p)
	{
		normal = n.Normalized();
		point = p;
		d = normal.DotProduct(p);
	}
	// Set the parameters of this Plane from given normal and distance from origin
	void SetNormalD(Vector n, gFloat d_)
	{
		normal = n.Normalized();
		d = d_;
	}
	// Set the parameters of this Plane from 3 given points on the plane (given in clockwise order)
	void Set3Points(Vector p1, Vector p2, Vector p3)
	{
		Vector v = p2 - p1, u = p3 - p1;
		normal = (v * u).Normalized();
		d = -(normal.DotProduct(p1));
		point = p1;
	}
	
	// Calculate the signed distance of a point from this Plane
	gFloat Distance(const Vector& p)
	{
		return normal.DotProduct(p) - d;
	}
	// Calculate the absolute distance of a point from this Plane
	gFloat AbsDistance(const Vector& p)
	{
		return Abs(normal.DotProduct(p) - d);
	}

	Vector point;	// Point on plane (optional)
	Vector normal;	// Plane normal (a,b,c)
	gFloat  d;		// Distance of Plane from origin
};
}	// namespace
#endif	// GLADE_PLANE_S