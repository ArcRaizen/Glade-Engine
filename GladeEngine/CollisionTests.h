#pragma once
#ifndef GLADE_COLLISION_TESTS_H
#define GLADE_COLLISION_TESTS_H

#ifndef GLADE_COLLIDER_H
#include "Collider.h"
#endif
#ifndef GLADE_CONTACT_H
#include "Contact.h"
#endif
#ifndef GLADE_VECTOR_H
#include "Math\Vector.h"
#endif
#ifndef GLADE_CORE_H
#include "Core.h"
#endif
#include <algorithm>

namespace Glade {
class CollisionTests
{
public:
	static int TestCollision(Collider* a, Collider* b, Contact* contacts);
	static bool AABBTest(AABB a, AABB b);

private:
	// Array of pointers to each Collision Test function
	typedef int (*FP)(Collider*, Collider*, Contact*);
	static const FP Tests[28];
	
	// Utility array. Stores indices in the Tests array.
	// Used to select index of correct function in Tests array
	// for Collider's being tested
	static const int helperIndices[7];

	static int SphereSphereTest(Collider* _a, Collider* b, Contact* contacts);
	static int SphereBoxTest(Collider* _a, Collider* b, Contact* contacts);
	static int SphereCylinderTest(Collider* _a, Collider* b, Contact* contacts);
	static int SphereConeTest(Collider* _a, Collider* b, Contact* contacts);
	static int SphereCapsuleTest(Collider* _a, Collider* b, Contact* contacts);
	static int SpherePlaneTest(Collider* _a, Collider* b, Contact* contacts);
	static int SphereMeshTest(Collider* _a, Collider* b, Contact* contacts);

	static int BoxBoxTest(Collider* _a, Collider* b, Contact* contacts);
	static int BoxCylinderTest(Collider* _a, Collider* b, Contact* contacts);
	static int BoxConeTest(Collider* _a, Collider* b, Contact* contacts);
	static int BoxCapsuleTest(Collider* _a, Collider* b, Contact* contacts);
	static int BoxPlaneTest(Collider* _a, Collider* b, Contact* contacts);
	static int BoxMeshTest(Collider* _a, Collider* b, Contact* contacts);

	static int CylinderCylinderTest(Collider* _a, Collider* b, Contact* contacts);
	static int CylinderConeTest(Collider* _a, Collider* b, Contact* contacts);
	static int CylinderCapsuleTest(Collider* _a, Collider* b, Contact* contacts);
	static int CylinderPlaneTest(Collider* _a, Collider* b, Contact* contacts);
	static int CylinderMeshTest(Collider* _a, Collider* b, Contact* contacts);

	static int ConeConeTest(Collider* _a, Collider* b, Contact* contacts);
	static int ConeCapsuleTest(Collider* _a, Collider* b, Contact* contacts);
	static int ConePlaneTest(Collider* _a, Collider* b, Contact* contacts);
	static int ConeMeshTest(Collider* _a, Collider* b, Contact* contacts);

	static int CapsuleCapsuleTest(Collider* _a, Collider* b, Contact* contacts);
	static int CapsulePlaneTest(Collider* _a, Collider* b, Contact* contacts);
	static int CapsuleMeshTest(Collider* _a, Collider* b, Contact* contacts);

	static int PlanePlaneTest(Collider* _a, Collider* b, Contact* contacts);
	static int PlaneMeshTest(Collider* _a, Collider* b, Contact* contacts);

	static int MeshMeshTest(Collider* _a, Collider* b, Contact* contacts);

// ~~~~ Utility Functions for Collision Detection ~~~~
	// Calculate the Closest Point on/in an OOBB to a Point p in space
	static Vector ClosestPointOnOOBB(Vector p, Vector c, Vector u[3], Vector e);

	// Calculate the squared distance between a Point 'p' and Line Segment 'ab'
	static gFloat SquaredDistanceOfPointFromLineSegment(Vector p, Vector a, Vector b);

	// Calculate closest point on Line Segment 'ab' to Point 'p'
	static Vector ClosestPointonLineSegment(Vector p, Vector a, Vector b);

	// Given Edge verticies a/b and normalized directions ua/ub,
	// calculate distance along 2 edges (s/t) to the points on each edge closest to the other edge
	static void ClosestPointsOnEdges(Vector a, Vector ua, Vector b, Vector ub, gFloat& s, gFloat &t);

	// Find all intersection points between 2D rectange with vertices at (+/-h[0], +/-h[1])
	// and 2D quadrilateral with vertices (p[0],p[1]), (p[2],p[3]), (p[4],p[5]), (p[6],p[7])
	// Intersection points returned as x,y pairs in 'ret' array
	// Return value is number of intersection points
	static int IntersectRectQuad(gFloat h[2], gFloat p[8], gFloat ret[16]);
};
} // namespace Glade
#endif // GLADE_COLLISION_TESTS_H

