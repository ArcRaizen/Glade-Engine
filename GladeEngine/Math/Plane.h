#pragma once
#ifndef GLADE_PLANE_S
#define GLADE_PLANE_S

#ifndef GLADE_VECTOR_H
#include "Vector.h"
#endif

namespace Glade {
struct Plane
{
	Plane(Vector n, Vector p) : normal(n.Normalized()), point(p), d(normal.DotProduct(point)) {}
	Plane(Vector n, float d_) : normal(n.Normalized()), d(d_) {}

	Vector point;
	Vector normal;
	float  d;
};
}	// namespace
#endif	// GLADE_PLANE_S