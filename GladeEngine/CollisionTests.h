#pragma once
#ifndef GLADE_COLLISION_TESTS_H
#define GLADE_COLLISION_TESTS_H

#ifndef GLADE_COLLIDER_H
#include "Collider.h"
#endif
#ifndef GLADE_CONTACT_H
#include "Contacts\Contact.h"
#endif
#ifndef GLADE_VECTOR_H
#include "Math\Vector.h"
#endif
#ifndef GLADE_RAY_S
#include "Math\Ray.h"
#endif
#ifndef GLADE_PLANE_S
#include "Math\Plane.h"
#endif
#ifndef GLADE_CONFIG_H
#include "GladeConfig.h"
#endif
#include <list>
#include <algorithm>

namespace Glade {
class CollisionTests
{
public:
	static int TestCollision(Collider* a, Collider* b, Contact* contacts);
	
	static void SetAABBTestEpsilon(gFloat e);
	static bool AABBTest(AABB a, AABB b);

	static bool RaySphereTest(Ray ray, Vector cen, gFloat r, gFloat& t);
	static bool RayAABBTest(Ray ray, AABB b, gFloat& t);
	static bool RayPlaneTest(Ray ray, Plane p, gFloat t, Vector& v);

private:
	// Array of pointers to each Collision Test function
	typedef int (*FP)(Collider*, Collider*, Contact*);
	static const FP Tests[28];
	
	// Utility array. Stores indices in the Tests array.
	// Used to select index of correct function in Tests array
	// for Collider's being tested
	static const int helperIndices[7];

	static gFloat AABBTestEpsilon;
	static gFloat EPADistanceThreshold;

	inline static gFloat GetCoeffOfRestitution(Collider* _a, Collider* _b) { return _a->physicMaterial->GetCombinedBounciness(_b->physicMaterial); }
	inline static gFloat GetStaticFriction(Collider* _a, Collider* _b) { return _a->physicMaterial->GetCombinedStaticFriction(_b->physicMaterial); }
	inline static gFloat GetDynamicFriction(Collider* _a, Collider* _b) { return _a->physicMaterial->GetCombinedDynamicFriction(_b->physicMaterial); }

	static int SphereSphereTest(Collider* _a, Collider* _b, Contact* contacts);
	static int SphereBoxTest(Collider* _a, Collider* _b, Contact* contacts);
	static int SphereCapsuleTest(Collider* _a, Collider* _b, Contact* contacts);
	static int SphereCylinderTest(Collider* _a, Collider* _b, Contact* contacts);
	static int SphereConeTest(Collider* _a, Collider* _b, Contact* contacts);
	static int SpherePlaneTest(Collider* _a, Collider* _b, Contact* contacts);

	static int BoxBoxTest(Collider* _a, Collider* _b, Contact* contacts);
	static int BoxCapsuleTest(Collider* _a, Collider* _b, Contact* contacts);
	static int BoxCylinderTest(Collider* _a, Collider* _b, Contact* contacts);
	static int BoxConeTest(Collider* _a, Collider* _b, Contact* contacts);
	static int BoxPlaneTest(Collider* _a, Collider* _b, Contact* contacts);

	static int CapsuleCapsuleTest(Collider* _a, Collider* _b, Contact* contacts);
	static int CapsuleCylinderTest(Collider* _a, Collider* _b, Contact* contacts);
	static int CapsuleConeTest(Collider* _a, Collider* _b, Contact* contacts);
	static int CapsulePlaneTest(Collider* _a, Collider* _b, Contact* contacts);

	static int CylinderCylinderTest(Collider* _a, Collider* _b, Contact* contacts);
	static int CylinderConeTest(Collider* _a, Collider* _b, Contact* contacts);
	static int CylinderPlaneTest(Collider* _a, Collider* _b, Contact* contacts);

	static int ConeConeTest(Collider* _a, Collider* _b, Contact* contacts);
	static int ConePlaneTest(Collider* _a, Collider* _b, Contact* contacts);

	static int PlanePlaneTest(Collider* _a, Collider* _b, Contact* contacts);
	static int PlaneMeshTest(Collider* _a, Collider* _b, Contact* contacts);

// ~~~~ GJK Collision Detection ~~~~ 
struct SupportPoint
{
	SupportPoint() {}
	SupportPoint(Collider* _a, Collider* _b, const Vector& d)
	{
		suppA = _a->GetSupportPoint(d);
		suppB = _b->GetSupportPoint(-d);
		p = suppA - suppB;
	}
	void Set(Collider* _a, Collider* _b, const Vector& d)
	{
		suppA = _a->GetSupportPoint(d);
		suppB = _b->GetSupportPoint(-d);
		p = suppA - suppB;
	}
	Vector p;				// Minkowski difference point
	Vector suppA, suppB;	// Individual support points (suppA - suppB = v)

	void operator=(const SupportPoint& other) { p = other.p; suppA = other.suppA; suppB = other.suppB; }
	bool operator==(const SupportPoint& other) { return p == other.p; }
	Vector operator-(const SupportPoint& other) const { return p - other.p; }
};

struct Triangle
{
	Triangle(const SupportPoint& _a, const SupportPoint& _b, const SupportPoint& _c) : a(_a), b(_b), c(_c) { normal = (b-a).CrossProduct(c-a).Normalized(); }
	SupportPoint a, b, c;
	Vector normal;
};

struct Edge
{
	Edge() {}
	Edge(SupportPoint _a, SupportPoint _b) : a(_a), b(_b) { }
	void Set(SupportPoint _a, SupportPoint _b) { a = _a; b = _b; }
	bool operator==(const Edge& other) { return a == other.a && b == other.b; }
	SupportPoint a, b;
};

	static int GJKTest(Collider* _a, Collider* _b, Contact* contacts);
	static bool GJKDoSimplex(SupportPoint simplex[4], unsigned int& simplexIndex, Vector& d);
	static int EPA(Collider* _a, Collider* _b, SupportPoint simplex[4], Contact* contacts);
	static void SetEPADistanceThreshold(gFloat dist);

// ~~~~ Utility Functions for Collision Detection ~~~~
	// Calculate the Closest Point on/in an OOBB to a Point p in space
	static Vector ClosestPointOnOOBB(Vector p, Vector c, Vector u[3], Vector e);

	// Calculate the squared distance between a Point 'p' and Line Segment 'ab'
	static gFloat SquaredDistanceOfPointFromLineSegment(Vector p, Vector a, Vector b);

	// Calculate closest point on Line Segment 'ab' to Point 'p'
	static Vector ClosestPointonLineSegment(Vector p, Vector a, Vector b, gFloat& t);

	// Given Edge verticies a/b and normalized directions ua/ub,
	// calculate distance along 2 edges (s/t) to the points on each edge closest to the other edge
	static void ClosestPointsOnEdges(Vector a, Vector ua, Vector b, Vector ub, gFloat& s, gFloat &t);

	// Find all intersection points between 2D rectangle with vertices at (+/-h[0], +/-h[1])
	// and 2D quadrilateral with vertices (p[0],p[1]), (p[2],p[3]), (p[4],p[5]), (p[6],p[7])
	// Intersection points returned as x,y pairs in 'ret' array
	// Return value is number of intersection points
	static int IntersectRectQuad(gFloat h[2], gFloat p[8], gFloat ret[16]);
};
} // namespace Glade
#endif // GLADE_COLLISION_TESTS_H