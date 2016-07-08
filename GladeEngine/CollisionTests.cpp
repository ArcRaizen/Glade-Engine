#include "CollisionTests.h"

using namespace Glade;

const CollisionTests::FP CollisionTests::Tests[28] = {
	&CollisionTests::SphereSphereTest, &CollisionTests::SphereBoxTest, &CollisionTests::SphereCapsuleTest, &CollisionTests::SphereCylinderTest,	&CollisionTests::SphereConeTest, &CollisionTests::SpherePlaneTest, &CollisionTests::GJKTest, 
	&CollisionTests::BoxBoxTest, &CollisionTests::GJKTest, &CollisionTests::GJKTest, &CollisionTests::GJKTest, &CollisionTests::BoxPlaneTest, &CollisionTests::GJKTest, 
	&CollisionTests::CapsuleCapsuleTest, &CollisionTests::GJKTest, &CollisionTests::GJKTest, &CollisionTests::CapsulePlaneTest, &CollisionTests::GJKTest,
	&CollisionTests::GJKTest, &CollisionTests::GJKTest, &CollisionTests::CylinderPlaneTest, &CollisionTests::GJKTest, 
	&CollisionTests::GJKTest, &CollisionTests::ConePlaneTest, &CollisionTests::GJKTest,  
	&CollisionTests::PlanePlaneTest, &CollisionTests::GJKTest, 
	&CollisionTests::GJKTest
	};

const int CollisionTests::helperIndices[7] = { 0, 6, 11, 15, 18, 20, 21 };

gFloat CollisionTests::AABBTestEpsilon = gFloat(0.03f);
gFloat CollisionTests::EPADistanceThreshold = gFloat(0.001f);

int CollisionTests::TestCollision(Collider* a, Collider* b, Contact* contacts)
{
	int _a = (int)a->GetShape(), _b = (int)b->GetShape();

	// Call appropriate function
	if(_a > _b)
		return Tests[helperIndices[_b] + _a](b, a, contacts);
	else
		return Tests[helperIndices[_a] + _b](a, b, contacts);
}

void CollisionTests::SetAABBTestEpsilon(gFloat e) { AABBTestEpsilon = e; }
bool CollisionTests::AABBTest(AABB a, AABB b)
{
	if(a.maximum.x < b.minimum.x - AABBTestEpsilon) return false;
	if(a.minimum.x > b.maximum.x + AABBTestEpsilon) return false;
	if(a.maximum.y < b.minimum.y - AABBTestEpsilon) return false;
	if(a.minimum.y > b.maximum.y + AABBTestEpsilon) return false;
	if(a.maximum.z < b.minimum.z - AABBTestEpsilon) return false;
	if(a.minimum.z > b.maximum.z + AABBTestEpsilon) return false;
	return true;
}

// Test if a Ray intersects with a Sphere
// If intersection, set 't' to distance along ray to intersection point and return True
// If no intersection, return False
bool CollisionTests::RaySphereTest(Ray ray, Vector cen, gFloat r, gFloat& t)
{
	// Ray: P + tV					(origin P, direction unit vector V, scalar t)
	// Circle: (X-C)(X-C) = r^2		(center C, radius r, any point on sphere X)
	// Assuming a Ray's direction vector is normalized, a point 't' distance along the ray intersects with the sphere according to the quadratic 
	//		t^2 + 2[(P-C).V]t + (P-C)(P-C)-r^2 = 0
	Vector diff = ray.origin - cen;
	gFloat b = diff.DotProduct(ray.dir);
	gFloat c = diff.DotProduct(diff) - r * r;

	// Exit if Ray's origin is outside sphere (c > 0)
	// and Ray is pointing away from sphere (b > 0)
	if(c > gFloat(0.0f) && b > gFloat(0.0f))
		return false;
	
	// Quadratic formula discriminant
	gFloat discr = b*b - c;

	// Negative discriminant means Ray misses Sphere
	if(discr < gFloat(0.0f))
		return false;

	// Solve for 't' (pick the smallest 't' or intersection closest to Ray origin)
	// (We probably don't care about the farther intersection)
	//		t = -[(P-C).V] +/- Sqrt( [(P-C).V]^2 - [(P-C)(P-C)-r^2] )
	t = -b - Sqrt(discr);

	// If 't' is negative, Ray started inside Sphere, clamp to 0
	if(t < gFloat(0.0f))
		t = 0.0f;
	return true;
}

// Test if a Ray intersects an AABB by computing the intersection intervals of the Ray with planes of
// the slabs that define the AABB (slabs being the space between 2 parallel planes)
// Track the farthest entry into and nearest exit out of the slabs
// If the farthest entry is ever farther than the nearest exit, there is no intersection
// If there is an intersection, set 't' to the distance along the Ray to the intersection point and return True
// If no intersection, return false
// Source: 'Real Time Collision Detection' by Christer Ericson, p-180-181
bool CollisionTests::RayAABBTest(Ray ray, AABB b, gFloat& t)
{
	t = 0.0f;
	gFloat tMax = ray.len;
	for(unsigned int i = 0; i < 3; ++i)
	{
		// Ray is parallel to slab, no hit if origin not in slab
		if(Abs(ray.dir[i]) < EPSILON)
		{
			if(ray.origin[i] < b.minimum[i] || ray.origin[i] > b.maximum[i])
				return false;
		}

		gFloat denom = gFloat(1.0f) / ray.dir[i];
		gFloat t1 = (b.minimum[i] - ray.origin[i]) * denom;
		gFloat t2 = (b.maximum[i] - ray.origin[i]) * denom;

		// T1 always intersection with near plane, t2 with far plane
		// Swap if necessary
		if(t1 > t2)
			Swap<gFloat>(t1, t2);

		// Compute intersection of slab intersection intervals
		t = Max(t,t1);
		tMax = Min(tMax, t2);

		// No collision if slab intersection becomes empty
		if(t > tMax)
			return false;
	}

	// Ray intersects all 3 slabs of AABB
	return true;
}

// Test if a Ray intersects a Plane.
// If intersection, set 't' to distance along ray to intersection point and return True
// If no intersection, return False
bool CollisionTests::RayPlaneTest(Ray ray, Plane p, gFloat t, Vector& v)
{
	gFloat dot = p.normal.DotProduct(ray.dir);
	if(dot < EPSILON)
		return false;

	t = (p.d - (p.normal.DotProduct(ray.origin))) / dot;
	if(t >= gFloat(0.0f) && ray.len > gFloat(0.0f) && t < ray.len)
		return true;

	return false;
}

// NORMAL IS RELATIVE TO _A
// CONTACT POINT SHOULD BE ON SURFACE OF _A
#pragma region Sphere Collisions
int CollisionTests::SphereSphereTest(Collider* _a, Collider* _b, Contact* contacts)
{
	SphereCollider* s1 = static_cast<SphereCollider*>(_a);
	SphereCollider* s2 = static_cast<SphereCollider*>(_b);

	// Collision true if distance between Sphere centers is less-than sum of their radii 
	// (Squared Distance between Sphere Centers is less-than square of sum of their radii)
	Vector diff = s2->position - s1->position;
	gFloat rad = s1->radius + s2->radius;
	if(diff.SquaredMagnitude() < rad * rad)
	{
		gFloat len = diff.Magnitude();
		Vector normal = diff / len;
		contacts->SetNewContact(s1->attachedBody, s2->attachedBody, GetCoeffOfRestitution(_a,_b), 
								GetStaticFriction(_a, _b), GetDynamicFriction(_a, _b), 
								normal, s1->position + normal*s1->radius, len - rad);
		return 1;
	}

	return 0;
}
int CollisionTests::SphereBoxTest(Collider* _a, Collider* _b, Contact* contacts)
{
	SphereCollider* s = static_cast<SphereCollider*>(_a);
	BoxCollider* b = static_cast<BoxCollider*>(_b);

	// Compute Closest Point on Box to Sphere Center
	Vector p = ClosestPointOnOOBB(s->position, b->position, b->u, b->halfWidths);

	// Collision true if distance between Sphere center and closest point on OOBB
	// is less-than the Sphere's radius
	Vector diff = p - s->position;
	if(diff.DotProduct(diff) < s->radius * s->radius)
	{
		gFloat len = diff.Magnitude();
		Vector normal = diff / len;
		contacts->SetNewContact(s->attachedBody, b->attachedBody, GetCoeffOfRestitution(_a,_b), 
								GetStaticFriction(_a, _b), GetDynamicFriction(_a, _b),
								normal,	s->position + normal*s->radius, len - s->radius);
		return 1;
	}
	return 0;
}
int CollisionTests::SphereCapsuleTest(Collider* _a, Collider* _b, Contact* contacts)
{
	SphereCollider* s = static_cast<SphereCollider*>(_a);
	CapsuleCollider* c = static_cast<CapsuleCollider*>(_b);

	// Calc distance between Sphere center and Line Segment between center points of Capsule's endcaps
	gFloat dist, rad = s->radius + c->radius;
	Vector d = ClosestPointonLineSegment(s->position, c->p, c->q, dist);

	// Collision is true if said distance is less than sum of Sphere+Capsule's radii
	if(dist * dist < rad * rad)
	{
		Vector normal = d - s->position;
		gFloat len = normal.Magnitude();
		normal /= len;
		contacts->SetNewContact(_a->attachedBody, _b->attachedBody, GetCoeffOfRestitution(_a, _b),
								GetStaticFriction(_a, _b), GetDynamicFriction(_a, _b),
								normal, s->position + normal*s->radius, len - rad);
		return 1;
	}
	return 0;
}
int CollisionTests::SphereCylinderTest(Collider* _a, Collider* _b, Contact* contacts)
{
	SphereCollider* s = static_cast<SphereCollider*>(_a);
	CylinderCollider* c = static_cast<CylinderCollider*>(_b);

	// Source: http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.49.9172&rep=rep1&type=pdf - Page 9/10
	// Sphere center projected along Cylinder axis
	gFloat dist = (s->position - c->p).DotProduct(c->axis); //
	Vector b = c->p + c->axis * dist;

	// Sphere lies between planes defined by ends of Cylinder
	// Collision true if distance between Sphere center and middle of Cylinder is
	// less-than the sum of their radii
	if(dist >= 0 && dist <= c->height)
	{
		Vector diff = b - s->position;
		gFloat rad = s->radius + c->radius;
		if(diff.SquaredMagnitude() < rad * rad)
		{
			gFloat len = diff.Magnitude();
			Vector norm = diff / len;
			contacts->SetNewContact(s->attachedBody, c->attachedBody, GetCoeffOfRestitution(_a, _b),
									GetStaticFriction(_a, _b), GetDynamicFriction(_a, _b), 
									norm, s->position + norm*s->radius, len - rad);
			return 1;
		}
	}

	// Distance along Cylinder axis of Sphere center from Cylinder
	gFloat d = dist > 0 ? dist - c->height : -dist;
	
	// Sphere totally passed planes defined by ends of Cylinder
	if(d >= s->radius) return 0;

	// Distance of....uh....look at the source
	// Distance from closest point on Sphere to Cylinder Surface and Sphere center
	gFloat p = Sqrt(s->radius*s->radius - d*d);

	// Collision true if distance between Sphere center and Cylinder axis
	// is less-than sum of Cylinder radius and distance from Cylinder surfance to closest point on Sphere
	Vector diff = b - s->position;
	gFloat rad = c->radius + p;
	if(diff.SquaredMagnitude() < rad * rad)
	{
		gFloat len = diff.Magnitude();
		Vector norm = diff / len;
		contacts->SetNewContact(s->attachedBody, c->attachedBody, GetCoeffOfRestitution(_a, _b),
								GetStaticFriction(_a, _b), GetDynamicFriction(_a, _b), 
								norm, s->position + norm*s->radius, len - rad);
		return 1;
	}
}
int CollisionTests::SphereConeTest(Collider* _a, Collider* _b, Contact* contacts)
{
	SphereCollider* s = static_cast<SphereCollider*>(_a);
	ConeCollider* c = static_cast<ConeCollider*>(_b);

	// Source: http://www.cbloom.com/3d/techdocs/culling.txt
	// http://www.gamedev.net/topic/555628-sphere-cone-test-with-no-sqrt-for-frustum-culling/
	// Image I drew to prove/check math: http://i.imgur.com/58j3Evq.png

	// Vector from Sphere center to Cone tip
	Vector v = s->position - c->tip;

	// Signed Distance of Sphere center along Cone axis
	gFloat a = v.DotProduct(c->axis);

	// Sphere totally behind Cone tip
	if(a < 0 && -a > c->radius)
		return 0;
	// Sphere totally passed Cone base
	else if(a > 0 && a - c->radius > c->height)
		return 0;

	// Radius of cone at 'a' units from tip along axis
	gFloat radAtA = a * (c->radius / c->height); // tan(theta) = radius/height

	// Distance from Sphere center to Cone axis
	gFloat distCenterToAxis = Sqrt(v.DotProduct(v) - a*a);

	// Distance from Sphere center to Cone surface (perpendicular to Cone axis)
	gFloat distCenterToSurface = distCenterToAxis - radAtA;

	// Shortest distance from Sphere enter to Cone surface
	gFloat cTheta = Cos(c->theta);
	gFloat e = distCenterToSurface * cTheta;
	
	// Collision true if shortest distance from Sphere center to Cone surface
	// is less than Sphere radius
	if(e < s->radius)
	{
		// Updated image to show this part: http://i.imgur.com/K6w9iSg.png
		Vector p = c->axis * (a + e*Sin(c->theta)) / (cTheta*cTheta);
		Vector normal = (p - s->position).Normalized();
		contacts->SetNewContact(s->attachedBody, c->attachedBody, GetCoeffOfRestitution(_a, _b),
								GetStaticFriction(_a, _b), GetDynamicFriction(_a, _b),
								normal, normal*s->radius, e - s->radius);
		return 1;
	}
	return 0;
}
int CollisionTests::SpherePlaneTest(Collider* _a, Collider* _b, Contact* contacts)
{
	SphereCollider* s = static_cast<SphereCollider*>(_a);
	PlaneCollider* p = static_cast<PlaneCollider*>(_b);

	// Compute distance of Sphere Center from Plane
	// Evaluating plane equation (n.Dot(X) = d) for a point gives signed distance of point to plane
	gFloat dist = s->position.DotProduct(p->normal) - p->d;

	// Collision true if distance from Sphere center to plane is less than Sphere's radius
	if(Abs(dist) < s->radius)
	{
		Vector normal = dist > 0 ? p->normal : -p->normal;
		contacts->SetNewContact(_a->attachedBody, _b->attachedBody, GetCoeffOfRestitution(_a, _b),
								GetStaticFriction(_a, _b), GetDynamicFriction(_a, _b),
								normal, s->position + normal*s->radius, Abs(dist) - s->radius);
		return 1;
	}
	return 0;
}
#pragma endregion

#pragma region Box Collisions
int CollisionTests::BoxBoxTest(Collider* _a, Collider* _b, Contact* contacts)
{
	BoxCollider* b1 = static_cast<BoxCollider*>(_a);
	BoxCollider* b2 = static_cast<BoxCollider*>(_b);

	/* Sources:
	'Real Time Collision Detection' by Christer Ericson, p101-105
	http://www.jkh.me/files/tutorials/Separating%20Axis%20Theorem%20for%20Oriented%20Bounding%20Boxes.pdf
	*/
	// Good ol' Separating Axis Theorem
	// http://bulletphysics.org/Bullet/BulletFull/btBoxBoxDetector_8cpp_source.html - Line 262-671

	// Vector to translate from A to B, then bring into a's coordinate frame
	Vector t = b2->position - b1->position;
	t = Vector(t.DotProduct(b1->u[0]), t.DotProduct(b1->u[1]), t.DotProduct(b1->u[2]));

	gFloat separationDist = -G_MAX, sep;
	Vector normal;
	bool invertNormal;
	int code = 0;		// Type of contact: 1,2,3 = b2 intersects face of b1
										//	4,5,6 = b1 intersects face of b2
										//  7..15 = edge-edge contact

	// Calculate the separation distance between the boxes along an axis
	// If there is separation, they do not intersect
	// Otherwise, track and save the smallest penetration depth and the corresponding normal
#define TEST(TL, sumRadii, norm, c)		\
	sep = Abs(TL) - (sumRadii);			\
	if(sep > 0) return 0;				\
	if(sep > separationDist)			\
	{									\
			separationDist = sep;		\
			normal = (norm);			\
			invertNormal = (TL) < 0;	\
			code = (c);					\
	}

	// Test axes L = A0, L = A1, L = A2
	Matrix R, AbsR;
	gFloat rb1, rb2, tDotL;
	for(unsigned int i = 0; i < 3; ++i)
	{
		// Compute Matrix to express b2 in b1's coordinate frame
		// Add in epsilon term to counteract errors when two edges are parallel and their
		// cross product is near null
		for(unsigned int j = 0; j < 3; ++j)
		{
			R(i,j) = b1->u[i].DotProduct(b2->u[j]);
			AbsR(i, j) = Abs(R(i,j)) + 1.0e-5f;
		}

		rb1 = b1->halfWidths[i];
		rb2 = b2->halfWidths.x * AbsR(i,0) + b2->halfWidths.y * AbsR(i,1) + b2->halfWidths.z * AbsR(i,2);
		tDotL = t[i];
		TEST(tDotL, rb1 + rb2, b1->u[i], i+1);
//		if(Abs(tDotL) > rb1 + rb2) return 0;
	}

	// Test axes L = B0, L = B1, L = B2
	for(unsigned int i = 0; i < 3; ++i)
	{
		rb1 = b1->halfWidths.x * AbsR(0, i) + b1->halfWidths.y * AbsR(1, i) + b2->halfWidths.z * AbsR(2, i);
		rb2 = b2->halfWidths[i];
		tDotL = t.x*R(0,i) + t.y*R(1,i) + t.z*R(2,i);
		TEST(tDotL, rb1 + rb2, b2->u[i], i+4);
//		if(Abs(tDotL) > rb1 + rb2) return 0;
	}

	gFloat l;
#undef TEST
#define TEST(TL, sumRadii, n1, n2, n3, c)				\
	sep = Abs(TL) - (sumRadii);							\
	if(sep > EPSILON) return 0;							\
	l = Sqrt((n1)*(n1) + (n2)*(n2) + (n3)*(n3));		\
	if(l > EPSILON)										\
	{													\
		sep /= l;										\
		if(sep * 1.05f > separationDist)				\
		{												\
			separationDist = sep;						\
			normal = Vector((n1)/l, (n2)/l, (n3)/l);	\
			invertNormal = (TL) < 0;					\
			code = (c);									\
		}												\
	}											


	// Test axis L = A0 x B0
	rb1 = b1->halfWidths.y * AbsR(2,0) + b1->halfWidths.z * AbsR(1, 0);
	rb2 = b2->halfWidths.y * AbsR(0,2) + b2->halfWidths.z * AbsR(0, 1);
	tDotL = t.z*R(1,0) - t.y*R(2,0);
	TEST(tDotL, rb1 + rb2, 0, -R(2,0), R(1,0), 7);
//	if(Abs(tDotL > rb1 + rb2)) return 0;

	// Test axis L = A0 x B1
	rb1 = b1->halfWidths.y * AbsR(2,1) + b1->halfWidths.z * AbsR(1,1);
	rb2 = b2->halfWidths.x * AbsR(0,2) + b2->halfWidths.z * AbsR(0,0);
	tDotL = t.z*R(1,1) - t.y*R(2,1);
	TEST(tDotL, rb1 + rb2, 0, -R(2,1), R(1,1), 8);
//	if(Abs(tDotL > rb1 + rb2)) return 0;

	// Test axis L = A0 x B2
	rb1 = b1->halfWidths.y * AbsR(2,2) + b1->halfWidths.z * AbsR(1,2);
	rb2 = b2->halfWidths.x * AbsR(0,1) + b2->halfWidths.y * AbsR(0,0);
	tDotL = t.z*R(1,2) - t.y*R(2,2);
	TEST(tDotL, rb1 + rb2, 0, -R(2,2), R(1,2), 9);
//	if(Abs(tDotL > rb1 + rb2)) return 0;

	// Test axis L = A1 x B0
	rb1 = b1->halfWidths.x * AbsR(2,0) + b1->halfWidths.z * AbsR(0,0);
	rb2 = b2->halfWidths.y * AbsR(1,2) + b2->halfWidths.z * AbsR(1,1);
	tDotL = t.x*R(2,0) - t.z*R(0,0);
	TEST(tDotL, rb1 + rb2, R(2,0), 0, -R(0,0), 10);
//	if(Abs(tDotL > rb1 + rb2)) return 0;

	// Test axis L = A1 x B1
	rb1 = b1->halfWidths.x * AbsR(2,1) + b1->halfWidths.z * AbsR(0,1);
	rb2 = b2->halfWidths.x * AbsR(1,2) + b2->halfWidths.z * AbsR(1,0);
	tDotL = t.x*R(2,1) - t.z*R(0,1);
	TEST(tDotL, rb1 + rb2, R(2,1), 0, -R(0,1), 11);
//	if(Abs(tDotL > rb1 + rb2)) return 0;

	// Test axis L = A1 x B2
	rb1 = b1->halfWidths.x * AbsR(2,2) + b1->halfWidths.z * AbsR(0,2);
	rb2 = b2->halfWidths.x * AbsR(1,1) + b2->halfWidths.y * AbsR(1,0);
	tDotL = t.x*R(2,2) - t.z*R(0,2);
	TEST(tDotL, rb1 + rb2, R(2,2), 0, -R(0,2), 12);
//	if(Abs(tDotL > rb1 + rb2)) return 0;

	// Test axis L = A2 x B0
	rb1 = b1->halfWidths.x * AbsR(1,0) + b1->halfWidths.y * AbsR(0,0);
	rb2 = b2->halfWidths.y * AbsR(2,2) + b2->halfWidths.z * AbsR(2,1);
	tDotL = t.y*R(0,0) - t.x*R(1,0);
	TEST(tDotL, rb1 + rb2, -R(1,0), R(0,0), 0, 13);
//	if(Abs(tDotL > rb1 + rb2)) return 0;

	// Test axis L = A2 x B1
	rb1 = b1->halfWidths.x * AbsR(1,1) + b1->halfWidths.y * AbsR(0,1);
	rb2 = b2->halfWidths.x * AbsR(2,2) + b2->halfWidths.z * AbsR(2,0);
	tDotL = t.y*R(0,1) - t.x*R(1,1);
	TEST(tDotL, rb1 + rb2, -R(1,1), R(0,1), 0, 14);
//	if(Abs(tDotL > rb1 + rb2)) return 0;

	// Test axis L = A2 x B2
	rb1 = b1->halfWidths.x * AbsR(1,2) + b1->halfWidths.y * AbsR(0,2);
	rb2 = b2->halfWidths.x * AbsR(2,1) + b2->halfWidths.y * AbsR(2,0);
	tDotL = t.y*R(0,2) - t.x*R(1,2);
	TEST(tDotL, rb1 + rb2, -R(1,2), R(0,2), 0, 15);
//	if(Abs(tDotL > rb1 + rb2)) return 0;

	// No separating axis found, Collision is true
	if(code == 0) return 0;
//	if(contacts == nullptr) return 1;

	// Compute normal in global coordinates
	if(normal.IsZero())
	{
		// Dot product with each column of b1's rotation matrix (where each row is its local axes)
		Vector n = normal;
		normal.x = n.DotProduct(Vector(b1->u[0].x, b1->u[1].x, b1->u[2].x));
		normal.y = n.DotProduct(Vector(b1->u[0].y, b1->u[1].y, b1->u[2].y));
		normal.z = n.DotProduct(Vector(b1->u[0].z, b1->u[1].z, b1->u[2].z));
	}
	if(invertNormal)
		normal.Negate();

	// Compute contact point(s)
	if(code > 6)	// Edge from b1 touching edge from b2
	{
		Vector p1 = b1->position, p2 = b2->position;
		gFloat sign;
		for(unsigned int i = 0; i < 3; ++i)
		{
			// Find point on intersecting edge of b1
			sign = normal.DotProduct(b1->u[i]) > 0 ? -1.0f : 1.0f;
			p1.x += sign * b1->halfWidths.x * b1->u[i].x;
			p1.y += sign * b1->halfWidths.y * b1->u[i].y;
			p1.z += sign * b1->halfWidths.z * b2->u[i].z;

			// Find point on intersecting edge of b2
			sign = normal.DotProduct(b2->u[i]) > 0 ? -1.0f : 1.0f;
			p2.x += sign * b2->halfWidths.x * b2->u[i].x;
			p2.y += sign * b2->halfWidths.y * b2->u[i].y;
			p2.z += sign * b2->halfWidths.z * b2->u[i].z;
		}
		// P1 and P2 now vertices at end of intersecting edges
		
		// Get direction of each edge (same as one of the box's axes)
		Vector ua = b1->u[(code-7)/3], ub = b2->u[(code-7)%3];

		// Calculate closest points on each edge to the other edge
		gFloat s, t;
		ClosestPointsOnEdges(p1, ua, p2, ub, s, t);
		p1 += ua*s;	// Move p1 down to closest point
		p2 += ub*t;	// Move p2 down to closest point

		// Contact point is halfway between 2 closest points
		contacts->SetNewContact(b1->attachedBody, b2->attachedBody, GetCoeffOfRestitution(_a,_b), 
								GetStaticFriction(_a, _b), GetDynamicFriction(_a, _b), 
								normal, ((p1+p2)*gFloat(0.5f)), -separationDist);
		return 1;
	}

	// Face-something intersection (separating axis is perpendicular to a face)
	// 'a' is reference face (normal is perpendicular to 'a')
	// 'b' is incident face (closest face on other box)

	// New variables to use - set so reference face is 'a' and incident face is 'b'
	Vector ua[3]={b1->u[0], b1->u[1], b1->u[2]}, ub[3]={b2->u[0], b2->u[1], b2->u[2]},	// axes of each box
		pa=b1->position, pb=b2->position,		// positions of each box
		sa=b1->halfWidths, sb=b2->halfWidths,	// halfwidths of each box ('s' for side)
		normal2 = normal, normalR, absNormalR;	// copy of normal and other useful vars
	if(code > 3)	// Swap them around
	{
		ua[0] = b2->u[0]; ua[1] = b2->u[1]; ua[2] = b2->u[2]; 
		ub[0] = b1->u[0]; ub[1] = b1->u[1]; ub[2] = b1->u[2];
		pa=b2->position; pb=b1->position; 
		sa=b2->halfWidths; sb=b1->halfWidths;
		normal2 = normal * gFloat(-1.0f);
	}

	// 'normalR' is normal vector of reference face dotted with axes of the incident box
	normalR[0] = ub[0].DotProduct(normal2);
	normalR[1] = ub[1].DotProduct(normal2);
	normalR[2] = ub[2].DotProduct(normal2);
	absNormalR[0] = Abs(normalR[0]);
	absNormalR[1] = Abs(normalR[1]);
	absNormalR[2] = Abs(normalR[2]);

	// Largest component of absNormalR corresponds to normal of incident face
	int lAbsNormalR, a1, a2;
	if(absNormalR.y > absNormalR.x)
	{
		a1 = 0;
		if(absNormalR.y > absNormalR.z)
		{	// Y largest component
			lAbsNormalR = 1;
			a2 = 2;
		}
		else
		{	// Z largest component
			a2 = 1;
			lAbsNormalR = 2;
		}
	}
	else if(absNormalR.x > absNormalR.z)
	{	// X largest component
		lAbsNormalR = 0;
		a1 = 1;
		a2 = 2;
	}
	else
	{	// Z largest compontn
		a1 = 0;
		a2 = 1;
		lAbsNormalR = 2;
	}

	// Calc center point of incident face in reference-face coordinates
	Vector center;
	if(normalR[lAbsNormalR] < 0)
		center = (pb - pa) + (ub[lAbsNormalR] * sb[lAbsNormalR]);
	else
		center = (pb - pa) - (ub[lAbsNormalR] * sb[lAbsNormalR]);

	// Find normal and non-normal axis numbers of the reference box
	int codeN, code1, code2;
	codeN = code <= 3  ? code-1 : code-4;	// axis of 'a' normal to reference face (0=x,1=y,2=z)
	code1 = codeN == 0 ? 1 : 0;
	code2 = codeN == 2 ? 1 : 2;

	// Find four corners of incident face in refrence-face coordinates
	gFloat quad[8];		// 2D coordinate of incident face (x,y pairs)
	gFloat c1, c2, m11, m12, m21, m22;
	gFloat k1, k2, k3, k4;
	c1 = center.DotProduct(ua[code1]);
	c2 = center.DotProduct(ua[code2]);
	if(code < 3)
	{
		m11 = R(code1, a1); //ua[code1].DotProduct(ub[a1]);
		m12 = R(code1, a2); //ua[code1].DotProduct(ub[a2]);
		m21 = R(code2, a1); //ua[code2].DotProduct(ub[a1]);
		m22 = R(code2, a2); //ua[code2].DotProduct(ub[a2]);
	}
	else
	{
		m11 = R(a1, code1);
		m12 = R(a2, code1);
		m21 = R(a1, code2);
		m22 = R(a2, code2);
	}
	k1 = m11 * sb[a1];
	k2 = m21 * sb[a1];
	k3 = m12 * sb[a2];
	k4 = m22 * sb[a2];
	quad[0] = c1 - k1 - k3;
	quad[1] = c2 - k2 - k4;
	quad[2] = c1 - k1 + k3;
	quad[3] = c2 - k2 + k4;
	quad[4] = c1 + k1 + k3;
	quad[5] = c2 + k2 + k4;
	quad[6] = c1 + k1 - k3;
	quad[7] = c2 + k2 - k4;

	// Find size of reference face
	gFloat rect[2] = {sa[code1], sa[code2]};

	// Intersect incident and reference faces
	gFloat ret[16];
	int n  = IntersectRectQuad(rect, quad, ret);
	if(n < 1) return 0;	// THIS SHOULD NEVER HAPPEN

	// Convert intersection points into reference-face coordinates,
	// and compute Contact position and depth for each point
	gFloat point[24];	// Contact points
	gFloat depth[8];	// corresponding depths
	gFloat det1 = gFloat(1.0f)/(m11*m22 - m12*m21);
	m11 *= det1;
	m12 *= det1;
	m21 *= det1;
	m22 *= det1;
	int cnum = 0;	// number of Contact points found
  	for(unsigned int i = 0; i < n; ++i)
	{
		gFloat k5 = (m22 * (ret[i*2] - c1)) - (m12 * (ret[i*2+1] - c2));
		gFloat k6 = (-m21 * (ret[i*2] - c1)) + (m11 * (ret[i*2+1] - c2));
		for(unsigned int j = 0; j < 3; ++j)
			point[cnum * 3 + j] = center[j] + (k5 * ub[a1][j]) + (k6 * ub[a2][j]);
		depth[cnum] = sa[codeN] - normal2.DotProduct(Vector(*(point+(cnum*3)), *(point+1+(cnum*3)), *(point+2+(cnum*3))));
		
		// Delete non-penetrating points in 'ret' by overwriting next point
		if(depth[cnum] >= 0)
		{
			ret[cnum*2] = ret[i*2];
			ret[cnum*2+1] = ret[i*2+1];
			++cnum;
		}
	}
	if(cnum < 1) return 0;	// THIS SHOULD NEVER HAPPEN
	
	// Determine the deepest penetrating contact points
	gFloat pen = 0.0f;
	for(unsigned int i = 0; i < cnum; ++i)
	{
		if(depth[i] > pen)
			pen = depth[i];
	}

	// Create Contacts
	Vector contactPoint;
	int cnumUsed = 0;
	if(code < 4)
	{
		for(unsigned int i = 0; i < cnum; ++i)
		{
			// Only use the most penetration Contact Point(s)
			// Should fix situation with tilted objects being detected penetrating on their higher/shallow end
			if(depth[i] == pen)
			{
				contactPoint += Vector(pa.x+normal.x*depth[i] + point[i*3], pa.y+normal.y*depth[i] + point[i*3+1], pa.z+normal.z*depth[i] + point[i*3+2]);
				++cnumUsed;
				//contacts->SetNewContact(b1->attachedBody, b2->attachedBody, 0.2f, 0.0f, normal, contactPoint, depth[i]);
				//contacts++;
			}
		}
	}
	else
	{
		for(unsigned int i = 0; i < cnum; ++i)
		{
			// Only use the most penetration Contact Point(s)
			// Should fix situation with tilted objects being detected penetrating on their higher/shallow end
			if(depth[i] == pen)
			{
				//contactPoint += Vector(pa.x-normal.x*depth[i] + point[i*3], pa.y-normal.y*depth[i] + point[i*3+1], pa.z-normal.z*depth[i] + point[i*3+2]);
				contactPoint += Vector(pa.x + point[i*3], pa.y + point[i*3+1], pa.z + point[i*3+2]);
				++cnumUsed;
				//contacts->SetNewContact(b1->attachedBody, b2->attachedBody, 0.2f, 0.0f, normal, contactPoint, depth[i]);
				//contacts++;
			}
		}
	}
	contacts->SetNewContact(b1->attachedBody, b2->attachedBody, GetCoeffOfRestitution(_a,_b), 
								GetStaticFriction(_a, _b), GetDynamicFriction(_a, _b), normal, contactPoint*(1.0f/(gFloat)cnumUsed), pen);
	//return cnumUsed;
   	return 1;
}
int CollisionTests::BoxCapsuleTest(Collider* _a, Collider* _b, Contact* contacts)
{
	BoxCollider* b = static_cast<BoxCollider*>(_a);
	CapsuleCollider* c = static_cast<CapsuleCollider*>(_b);

	return 0;
}
int CollisionTests::BoxCylinderTest(Collider* _a, Collider* _b, Contact* contacts)
{
	BoxCollider* b = static_cast<BoxCollider*>(_a);
	CylinderCollider* c = static_cast<CylinderCollider*>(_b);

	return 0;
}
int CollisionTests::BoxConeTest(Collider* _a, Collider* _b, Contact* contacts)
{
	BoxCollider* b = static_cast<BoxCollider*>(_a);
	ConeCollider* c = static_cast<ConeCollider*>(_b);

	return 0;
}
int CollisionTests::BoxPlaneTest(Collider* _a, Collider* _b, Contact* contacts)
{
	BoxCollider* b = static_cast<BoxCollider*>(_a);
	PlaneCollider* p = static_cast<PlaneCollider*>(_b);

	// Compute 'radius' of box along plane normal
	gFloat r = (b->halfWidths.x * Abs(p->normal.DotProduct(b->u[0]))) +
				(b->halfWidths.y * Abs(p->normal.DotProduct(b->u[1]))) +
				(b->halfWidths.z * Abs(p->normal.DotProduct(b->u[2])));

	// Compute distance from Box center to plane
	// Evaluating plane equation (n.Dot(X) = d) for a point gives signed distance of point to plane
	gFloat d = b->position.DotProduct(p->normal) - p->d;

	// Collision true if distance from Box to plane is less-than Box 'radius'
	if(Abs(d) < r)
	{
		Vector normal = d > 0 ? p->normal : -p->normal;
		contacts->SetNewContact(_a->attachedBody, _b->attachedBody, GetCoeffOfRestitution(_a, _b),
								GetStaticFriction(_a, _b), GetDynamicFriction(_a, _b),
								normal, b->position + normal*r, d - r);
		return 1;
	}
	return 0;
}
#pragma endregion


#pragma region Capsule Collision
int CollisionTests::CapsuleCapsuleTest(Collider* _a, Collider* _b, Contact* contacts)
{
	CapsuleCollider* c1 = static_cast<CapsuleCollider*>(_a);
	CapsuleCollider* c2 = static_cast<CapsuleCollider*>(_b);

	// Get (squared) distances between each Capsule endpoint and the other cylidner's rotation axis
	gFloat d1 = SquaredDistanceOfPointFromLineSegment(c1->p, c2->p, c2->q),
			d2 = SquaredDistanceOfPointFromLineSegment(c1->q, c2->p, c2->q),
			d3 = SquaredDistanceOfPointFromLineSegment(c2->p, c1->p, c1->q),
			d4 = SquaredDistanceOfPointFromLineSegment(c2->q, c1->p, c2->q);
	
	// Collision true if shortest distance between Capsules is less than the sum of their radii
	gFloat min = Min<gFloat>(Min<gFloat>(Min<gFloat>(d1, d2), d3), d4);
	return min < (c1->radius+c2->radius) * (c1->radius+c2->radius);
}
int CollisionTests::CapsuleCylinderTest(Collider* _a, Collider* _b, Contact* contacts)
{
	CapsuleCollider* c1 = static_cast<CapsuleCollider*>(_a);
	CylinderCollider* c2 = static_cast<CylinderCollider*>(_b);

	return 0;
}
int CollisionTests::CapsuleConeTest(Collider* _a, Collider* _b, Contact* contacts)
{
	CapsuleCollider* c1 = static_cast<CapsuleCollider*>(_a);
	ConeCollider* c2 = static_cast<ConeCollider*>(_b);

	return 0;
}
int CollisionTests::CapsulePlaneTest(Collider* _a, Collider* _b, Contact* contacts)
{
	CapsuleCollider* c = static_cast<CapsuleCollider*>(_a);
	PlaneCollider* p = static_cast<PlaneCollider*>(_b);

	// Compute distance of center of each of Capsule's half-Sphere caps from plane
	gFloat dist1 = c->p.DotProduct(p->normal) - p->d;
	gFloat dist2 = c->q.DotProduct(p->normal) - p->d;

	// Collision true if distance from center of half-Sphere caps is less than their radii
	if(Abs(dist1) < c->radius) return true;
	if(Abs(dist2) < c->radius) return true;

	return 0;
}
#pragma endregion


#pragma region Cylinder Collisions
int CollisionTests::CylinderCylinderTest(Collider* _a, Collider* _b, Contact* contacts)
{
	CylinderCollider* c1 = static_cast<CylinderCollider*>(_a);
	CylinderCollider* c2 = static_cast<CylinderCollider*>(_b);

	return 0;
}
int CollisionTests::CylinderConeTest(Collider* _a, Collider* _b, Contact* contacts)
{
	CylinderCollider* c1 = static_cast<CylinderCollider*>(_a);
	ConeCollider* c2 = static_cast<ConeCollider*>(_b);

	return 0;
}
int CollisionTests::CylinderPlaneTest(Collider* _a, Collider* _b, Contact* contacts)
{
	CylinderCollider* c = static_cast<CylinderCollider*>(_a);
	PlaneCollider* p = static_cast<PlaneCollider*>(_b);

	return 0;
}
#pragma endregion

#pragma region Cone Collision
int CollisionTests::ConeConeTest(Collider* _a, Collider* _b, Contact* contacts)
{
	ConeCollider* c1 = static_cast<ConeCollider*>(_a);
	ConeCollider* c2 = static_cast<ConeCollider*>(_b);

	return 0;
}
int CollisionTests::ConePlaneTest(Collider* _a, Collider* _b, Contact* contacts)
{
	ConeCollider* c = static_cast<ConeCollider*>(_a);
	PlaneCollider* p = static_cast<PlaneCollider*>(_b);

	// Check collision with tip of cone
	// Collision true if signed distance of Cone tip to plane is negative (tip is in negative halfspace of Plane)
	if(c->tip.DotProduct(p->normal) - p->d < 0)
		return true;

	// Check collision with 'Q' - point on edge of the Cone base, farthest in direction of -Plane.normal
	// Compute 'Q'
	Vector q = c->tip + c->axis*c->height + (p->normal.CrossProduct(c->axis)).CrossProduct(c->axis) * c->radius;

	// Collision is true if signed distance from Q to plane is negative (Q is in the negative halfspace of Plane)
	return q.DotProduct(p->normal) - p->d < 0;
}
#pragma endregion

#pragma region PlaneCollision
int CollisionTests::PlanePlaneTest(Collider* _a, Collider* _b, Contact* contacts)
{
	PlaneCollider* p1 = static_cast<PlaneCollider*>(_a);
	PlaneCollider* p2 = static_cast<PlaneCollider*>(_b);

	// Collision true if Planes are not parallel (cross product of their normals is 0)
	return !p1->normal.CrossProduct(p2->normal).IsZero();
}
int CollisionTests::PlaneMeshTest(Collider* _a, Collider* _b, Contact* contacts)
{
	PlaneCollider* p = static_cast<PlaneCollider*>(_a);
	MeshCollider* m = static_cast<MeshCollider*>(_b);

	return 0;
}
#pragma endregion

int CollisionTests::GJKTest(Collider* _a, Collider* _b, Contact* contacts)
{
	// Array of 4 vertices representing a k-Simplex as k goes from 0 to 3
	// Always arranged so the largest index is the newest vertex 'a' and the smallest index is the oldest vertex ('b', 'c', or 'd')
	SupportPoint simplex[4];
	Vector d = (_b->position - _a->position).Normalized();	// starting search direction
	SupportPoint supp;
	unsigned int simplexIndex = 0;

	// Add 1st point on Minkowski Difference to Simplex
	simplex[simplexIndex++].Set(_a, _b, d);
	d.Negate();

	while(true)
	{
		// Add new point to Simplex
		supp.Set(_a, _b, d);

		// If last point in Simplex does not pass origin in direction 'd,' then the
		// Minkowski Difference cannot possibly contain the origin, therefore no collision.
		if(supp.p.DotProduct(d) <= gFloat(0.0f))
			return 0;

		// Add new point to Simplex
		simplex[simplexIndex++] = supp;

		if(GJKDoSimplex(simplex, simplexIndex, d))
		{
			// Tetrahedral Simplex contains origin, collision confirmed
			// Run Expanding Polytope Algorithm (EPA) to get collision details
			return EPA(_a, _b, simplex, contacts);
		}
		// No collision found yet, keep going
	}
	return 0;
}
// https://mollyrocket.com/849
bool CollisionTests::GJKDoSimplex(SupportPoint simplex[4], unsigned int& simplexIndex, Vector& d)
{
	switch(simplexIndex)
	{
		case 2:		// 1-Simplex - Line Segment
		{
			Vector ab = simplex[0] - simplex[1], ao = simplex[1].p.Negated();

			// AB points in direction towards origin, search for next point
			// in direction towards origin & perpendicular to AB
			if(ab.DotProduct(ao) > 0)
				d  = (ab.CrossProduct(ao)).CrossProduct(ab);
			else	// AB not in direction towards origin, throwout B
			{		// search directly toward origin now
				simplex[0] = simplex[1];
				simplexIndex = 1;
				d = ao;
			}
			return false;
		}
		case 3:		// 2-Simplex - Triangle
		{	// Test which Voronoi region of the triangle with origin is in
			// If not in region inside triangle, throwout the remaining point(s) and
			// restart the Simplex with the corresponding point(s) and a new search direction
			Vector ab = simplex[1] - simplex[2], ac = simplex[0] - simplex[2], ao = simplex[2].p.Negated();
			Vector abc = ab.CrossProduct(ac); // triangle normal

			if(abc.CrossProduct(ac).DotProduct(ao) > 0)	// Test if in region outside segment AC
			{
				if(ac.DotProduct(ao) > 0)	// Test if AC points toward origin at all
				{
					// It does, throw out point 'B', restart Simpelex with segment AC and
					// new search direction perpendicular to AC away from triangle
					simplex[1] = simplex[2];
					simplexIndex = 2;
					d = (ac.CrossProduct(ao)).CrossProduct(ac);
				}
				// AC doesn't point toward origin
				else if(ab.DotProduct(ao) > 0)	// Test if AB does instead
				{
					// It does, throw out point 'C', restart Simplex with segment AB and
					// new search direction perpendicular to AB away from triangle
					simplex[0] = simplex[1];
					simplex[1] = simplex[2];
					simplexIndex = 2;
					d = (ab.CrossProduct(ao)).CrossProduct(ab);
				}
				else	// AB doesn't point toward origin either, restart Simplex with just A
				{
					simplex[0] = simplex[2];
					simplexIndex = 1;
					d = ao;
				}
			}
			else	// Origin in region inside segment AC
			{
				if(ab.CrossProduct(abc).DotProduct(ao) > 0)		// Test if in region outside sement AB
				{
					if(ab.DotProduct(ao) > 0)	// Test of AB points toward origin at all
					{
						// It does,, throwout point 'C', restart Simplex with segment AB and
						// new search direction perpendicular to AB away from triangle
						simplex[0] = simplex[1];
						simplex[1] = simplex[2];
						simplexIndex = 2;
						d = (ab.CrossProduct(ao)).CrossProduct(ab);
					}
					else	// AB doesn't point toward origin, restart Simplex with just A
					{
						simplex[0] = simplex[2];
						simplexIndex = 1;
						d = ao;
					}
				}
				else	// Origin not outside segment AB, so its inside both AC and AB
				{
					// We can build a tetrahedron with a 4th point now
					if(abc.DotProduct(ao) > 0)	// Search "above" triangle
						d = abc;
					else						// Or "below" triangle
					{
						// Requires reorder of points so they are still clockwise
						SupportPoint temp = simplex[0];
						simplex[0] = simplex[1];
						simplex[1] = temp;
						d = -abc;
					}
				}
			}
			
			return false;
		}
		case 4:	// 3-Simplex - Tetrahedron	
		{
			Vector ab = simplex[2]-simplex[3], ac = simplex[1]-simplex[3], ad = simplex[0]-simplex[3], ao = simplex[3].p.Negated();
			Vector abcN = ab.CrossProduct(ac);
			Vector acdN = ac.CrossProduct(ad);
			Vector adbN = ad.CrossProduct(ab);

			// Tests for each new plane of Tetrahedron (plane BCD doesn't need to be tested)
			int planeTests = 
#ifdef LEFT_HANDED_COORDS
					abcN.DotProduct(ao) > 0 ? 0x1 : 0 | acdN.DotProduct(ao) > 0 ? 0x2 : 0 | adbN.DotProduct(ao) > 0 ? 0x4 : 0;
#else
					ao.DotProduct(abcN) > 0 ? 0x1 : 0 | ao.DotProduct(acdN) > 0 ? 0x2 : 0 | ao.DotProduct(adbN) > 0 ? 0x4 : 0;
#endif
			bool skip = false;
			switch(planeTests)
			{
				case 0:					// Origin inside all planes that compose Tetrahedron, we have proven collision
					return true;
				case 0x1:				// Origin only outside ABC
					// re-arrage Simplex so ABC is 1st 3 points (already done)
					skip = true;
				case 0x2:				// Origin only outside ACD
					if(!skip)
					{
						// re-arrange Simplex so ACD is 1st 3 points
						simplex[2] = simplex[1];
						simplex[1] = simplex[0];

						// re-arrange Simplex edges accordingly
						ab = ac;
						ac = ad;
						abcN = acdN;	// set face normal to 'abc' too
						skip = true;
					}
				case 0x4:				// Origin only outisde ADB
					if(!skip)
					{
						// re-arrange Simplex so ADB is 1st 3 points
						simplex[1] = simplex[2];
						simplex[2] = simplex[0];

						// re-arrange Simplex edges accordingly
						ac = ab;
						ab = ad;
						abcN = adbN;	// set face normal to 'abcN' too
					}

					// Simplex and data now rearranged so nearest face to origin is 3 most recent points
					// in Simplex, the edges are saved to 'ab' and 'ac' and the normal is saved to 'abc'
					// Test which feature (2 edges or face) is nearest to the origin
					if(abcN.CrossProduct(ac).DotProduct(ao) > 0)		// Is segment 'ac' closest feature?
					{
						// 'ac' is nearest feature, reset Simplex to segment 'ac'
						simplex[0] = simplex[1];
						simplex[1] = simplex[4];
						d = ac.CrossProduct(ao);
						simplexIndex = 2;
						return false;
					}
					else if(ab.CrossProduct(abcN).DotProduct(ao) > 0) // Is segment 'ab' closest feature?
					{
						// 'ab' is nearest feature, reset Simplex to segment 'ab'
						simplex[0] = simplex[2];
						simplex[1] = simplex[3];
						d = ab.CrossProduct(ao);
						simplexIndex = 2;
						return false;
					}

					// Face is closest feature, reset Simplex to plane 'abc'
					simplex[0] = simplex[1];
					simplex[1] = simplex[2];
					simplex[2] = simplex[3];
					d = abcN;
					simplexIndex = 3;
					return false;
				case 0x1 | 0x2:			// Origin only inside 1 plane, outside ABC and ACD
					skip = true;
				case 0x1 | 0x4:			// Origin only inside 1 plane, outside ADB and ABC
					if(!skip)
					{
						// Re-arrange simplex... 
						SupportPoint temp = simplex[1];
						simplex[1] = simplex[2];
						simplex[2] = simplex[0];
						simplex[0] = temp;

						// and edges...
						Vector temp2 = ac;
						ac = ab;
						ab = ad;
						ad = temp2;

						// and face normals.
						acdN = abcN;
						abcN = adbN;
					}
				case 0x2 | 0x4:			// Origin only inside 1 plane, outside ACD and ADB
					if(!skip)
					{
						// Re-arrange simplex...
						SupportPoint temp = simplex[2];
						simplex[2] = simplex[1];
						simplex[1] = simplex[0];
						simplex[0] = temp;

						// and edges...
						Vector temp2 = ab;
						ab = ac;
						ac = ad;
						ad = temp2;

						// and face normals.
						abcN = acdN;
						acdN = adbN;
					}
					// Simplex is now arranged so that the 2 faces the origin lies outside of are in 'abc' and 'acd'
					// the edge shared by both faces is 'ac,' and the normals are 'abcN' and 'acdN'

					// Origin is outside 2 faces of Tetrahedron. 
					// Test if segment 'ac' (the segment between faces ABC and ACD) is closer than face ABC
					if(abcN.CrossProduct(ac).DotProduct(ao) > 0)
					{
						// Yes. Segment 'ac' is closer, so we now have to check face ACD.
						// Re-arrage Simplex so ACD is 1st 3 points
						simplex[2] = simplex[1];
						simplex[1] = simplex[0];

						// re-arrange edges accordingly
						ab = ac;
						ac = ad;
						abcN = acdN;	// set face normal to 'abc' too
						skip = false;	// test segment 'ac' then 'ab' if necessary
					}
					else // Face 'abc' is closer to origin than segment 'ac.' Segment 'ac' was already tested, skip to 'ab' test.
						skip = true;	// skip testing 'ac', only test 'ab'

					// Ok, at this stage, the origin is outside face 'abc'
					// Test edges 'ac' and 'ab.' If neither of them is the closest feature, then face 'abc' is
					if(!skip && abcN.CrossProduct(ac).DotProduct(ao) > 0)		// Is segment 'ac' closest feature?
					{
						// 'ac' is nearest feature, reset Simplex to segment 'ac'
						simplex[0] = simplex[1];
						simplex[1] = simplex[4];
						d = ac.CrossProduct(ao);
						simplexIndex = 2;
						return false;
					}
					else if(ab.CrossProduct(abcN).DotProduct(ao) > 0) // Is segment 'ab' closest feature?
					{
						// 'ab' is nearest feature, reset Simplex to segment 'ab'
						simplex[0] = simplex[2];
						simplex[1] = simplex[3];
						d = ab.CrossProduct(ao);
						simplexIndex = 2;
						return false;
					}

					// Face is closest feature, reset Simplex to plane 'abc'
					simplex[0] = simplex[1];
					simplex[1] = simplex[2];
					simplex[2] = simplex[3];
					d = abcN;
					simplexIndex = 3;
					return false;

				default:				// Origin not inside any face.....oops
					AssertMsg(false, "Now you fucked up...");
					return false;
			}
		}
		default: { return false; }
	}
}

void CollisionTests::SetEPADistanceThreshold(gFloat dist) { EPADistanceThreshold = dist; }
int CollisionTests::EPA(Collider* _a, Collider* _b, SupportPoint simplex[4], Contact* contacts)
{
	std::list<Triangle> triangles;
	std::list<Edge> edges;
	std::list<Triangle>::iterator face;
	std::list<Edge>::iterator eIter;
	SupportPoint newPoint;
	Edge e;
	gFloat dist = G_MAX, prevDist = 0;

	triangles.push_back(Triangle(simplex[3], simplex[2], simplex[1]));
	triangles.push_back(Triangle(simplex[3], simplex[1], simplex[0]));
	triangles.push_back(Triangle(simplex[3], simplex[0], simplex[2]));
	triangles.push_back(Triangle(simplex[2], simplex[0], simplex[1]));

	while(true)
	{
		// Pick closest 'face' (triangle) to origin
		gFloat d;
		for(auto iter = triangles.begin(); iter != triangles.end(); ++iter)
		{
			d = (*iter).normal.DotProduct((*iter).a.p);
			if(d < dist)
			{
				dist = d;
				face = iter;
			}
		}

		// Check if the face we chose is sufficiently close to the border of the Minkowski Difference
		// If so, end now, we have the info we're looking for
		if(dist - prevDist <= EPADistanceThreshold)
			break;

		// Get new support point from chosen face's normal
		newPoint.Set(_a, _b, (*face).normal);

		// New support point isn't any further away, it's on the nearest face
		// Nearest face must be on edge of Minkowski Difference
		if(face->normal.DotProduct(newPoint.p) == dist)
			break;

		// Get list of faces "seen" by new support point
		// Face is "seen" if new support point is in positive halfspace of plane containing face
		// (erase_if and fancy lambda to move all "seen" faces to end of triangles vector
		auto iter = std::remove_if(triangles.begin(), triangles.end(), 
				[&](Triangle t) { return t.normal.DotProduct(newPoint.p - t.a.p) > 0; });

		// Add edges of each "seen" face to edges list
		// If opposite edge already in list, don't add and remove opposite edge
		// Fancy lambda again to handle testing/adding edges
		auto addEdgeLambda = [&](const SupportPoint& a, SupportPoint& b){
						for(auto iter = edges.begin(); iter != edges.end(); ++iter)	{
							if(iter->a == b && iter->b == a) {
								edges.erase(iter);
								return;
							}
						}
						edges.emplace_back(a,b);
		};
		for(auto i = iter; i != triangles.end(); ++i)
		{
			addEdgeLambda(i->a, i->b);
			addEdgeLambda(i->b, i->c);
			addEdgeLambda(i->c, i->a);
		}

		// Remove "seen" faces from list
		triangles.erase(iter, triangles.end());

		// Remove chosen face from polytope
//		triangles.erase(face);

		// Add new faces to cover the "hole" made from the removed faces
		for(auto iter = edges.begin(); iter != edges.end(); ++iter)
		{
			Triangle t(newPoint, iter->a, iter->b);
			triangles.push_back(t);
			gFloat g = t.normal.DotProduct(t.a.p);
			g += 1;
		}

		// Clear edge list for next iteration
		edges.clear();
		prevDist = dist;
		dist = G_MAX;
	}

	// Polytope fully expanded, extract collision info
	// Get barycentric coordinates of origin projected onto nearest face
	// Taken from "Real Time Collision Detection" by Christer Ericson, p47-48
	Vector v0 = face->b - face->a, v1 = face->c - face->a, v2 = face->a.p.Negated();
	gFloat d00 = v0.DotProduct(v0);
	gFloat d01 = v0.DotProduct(v1);
	gFloat d11 = v1.DotProduct(v1);
	gFloat d20 = v2.DotProduct(v0);
	gFloat d21 = v2.DotProduct(v1);
	gFloat denom = d00 * d11 - d01 * d01;
	gFloat v = (d11 * d20 - d01 * d21) / denom;
	gFloat w = (d00 * d21 - d01 * d20) / denom;
	gFloat u = gFloat(1.0f) - v - w;

	contacts->SetNewContact(_a->attachedBody, _b->attachedBody, GetCoeffOfRestitution(_a, _b),
							GetStaticFriction(_a, _b), GetDynamicFriction(_a, _b), face->normal,
							face->a.suppA*u + face->b.suppA*v + face->c.suppA*w,
							dist);
	return 1;
}

#pragma region Utility Functions
Vector CollisionTests::ClosestPointOnOOBB(Vector p, Vector c, Vector u[3], Vector e)
{
	Vector d = p - c;
	Vector q = c;	// Start result at center of box, step from there

	gFloat dist;
	for(unsigned int i = 0; i < 3; ++i)
	{
		// Project 'd' onto each of the box's axes
		// to get distance of 'd' from box's center along each axis
		dist = d.DotProduct(u[i]);

		// Clamp distance along axis to sides of box
		if(dist > e[i]) dist = e[i];
		if(dist < -e[i]) dist = -e[i];

		// Step that dist along the axis to get closest point along axis
		q += u[i] * dist;
	}

	return q;
}

gFloat CollisionTests::SquaredDistanceOfPointFromLineSegment(Vector p, Vector a, Vector b)
{
	// Source: 'Real Time Collision Detection' by Christer Ericson, p-129-130
	Vector ab = b - a, ap = p - a;

	// Length of projection of 'p' onto 'ab'
	gFloat e = ap.DotProduct(ab);

	// 'p' projects outside 'ab'
	if(e < 0) return ap.DotProduct(ap);	// projects behind 'a'
	gFloat f = ab.DotProduct(ab);
	Vector bp = p - b;
	if(e > f) return bp.DotProduct(bp);	// projects passed 'b'

	// 'p' projects onto ab
	return (ap.DotProduct(ap)) - (e * e / f);
}

Vector CollisionTests::ClosestPointonLineSegment(Vector p, Vector a, Vector b, gFloat& t)
{
	// Source: 'Real Time Collision Detection' by Christer Ericson, p-128
	Vector ab = b - a;

	// Project 'p' onto 'ab', computing parameterized position d(t) = a + t*(b-a)
	t = (p - a).DotProduct(ab) / ab.DotProduct(ab);

	// If outside segment, clamp to 'ab'
	if(t < 0.0f) return a;
	else if(t > 1.0f) return b;

	return a + (ab * t);
}

// Adapted from http://bulletphysics.org/Bullet/BulletFull/btBoxBoxDetector_8cpp_source.html#l00086 Line 86
void CollisionTests::ClosestPointsOnEdges(Vector a, Vector ua, Vector b, Vector ub, gFloat& s, gFloat &t)
{
	Vector p = b - a;
	gFloat uaub = ua.DotProduct(ub);
	gFloat q1 = ua.DotProduct(p);		// Length of 'p' projected onto 'ua'
	gFloat q2 = -ub.DotProduct(p);		// Length of 'p' projected onto 'ub'
	gFloat d = 1 - uaub*uaub;
	if(d <= 1.0e-3f)
		s = t = 0.0f;
	else
	{
		d = 1.0f / d;
		s = (q1 + uaub*q2) * d;
		t = (uaub*q1 + q2) * d;
	}
}

int CollisionTests::IntersectRectQuad(gFloat h[2], gFloat p[8], gFloat ret[16])
{
	// q and r contain nq and nr coordinate points for current and chopped polygons
	int nq = 4, nr = 0;
	gFloat buffer[16];
	gFloat* q = p;
	gFloat* r = ret;

	for(unsigned int dir = 0; dir <= 1; ++dir)
	{	// direction notation: xy[0] = x-axis, xy[1] = y-axis
		for(int sign = -1; sign <= 1; sign +=2)
		{	// chop q along line xy[dir] = sign*h[dir]
			gFloat* pq = q;
			gFloat* pr = r;
			nr = 0;
			
			for(int i = nq; i > 0; --i)
			{	// go through all points in 'q' and all lines between adjacent points
				if(sign * pq[dir] < h[dir])
				{	// this point is inside the chopping line
					pr[0] = pq[0];
					pr[1] = pq[1];
					pr += 2;
					nr++;
					if(nr & 8)
					{
						q = r;
						goto done;
					}
				}

				gFloat* nextQ = (i > 1) ? pq+2 : q;
				if((sign * pq[dir] < h[dir]) ^ (sign * nextQ[dir] < h[dir]))
				{	// this line crosses the chopping line
					pr[1-dir] = pq[1-dir] + (nextQ[1-dir] - pq[1-dir]) / 
						(nextQ[dir] - pq[dir]) * (sign * h[dir] - pq[dir]);
					pr[dir] = sign * h[dir];
					pr += 2;
					nr++;
					if(nr & 8)
					{
						q = r;
						goto done;
					}
				}
				pq += 2;
			}
			q = r;
			r = (q == ret) ? buffer : ret;
			nq = nr;
		}
	}
done:
	if(q != ret) memcpy(ret, q, nr*2*sizeof(gFloat));
	return nr;
}
#pragma endregion