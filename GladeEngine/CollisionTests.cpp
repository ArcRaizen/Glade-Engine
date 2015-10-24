#include "CollisionTests.h"

using namespace Glade;

const CollisionTests::FP CollisionTests::Tests[28] = {
	&CollisionTests::SphereSphereTest, &CollisionTests::SphereBoxTest, &CollisionTests::SphereCylinderTest, &CollisionTests::SphereConeTest, 
	&CollisionTests::SphereCapsuleTest, &CollisionTests::SpherePlaneTest, &CollisionTests::SphereMeshTest, &CollisionTests::BoxBoxTest, 
	&CollisionTests::BoxCylinderTest, &CollisionTests::BoxConeTest, &CollisionTests::BoxCapsuleTest, &CollisionTests::BoxPlaneTest, 
	&CollisionTests::BoxMeshTest, &CollisionTests::CylinderCylinderTest, &CollisionTests::CylinderConeTest, &CollisionTests::CylinderCapsuleTest, 
	&CollisionTests::CylinderPlaneTest, &CollisionTests::CylinderMeshTest, &CollisionTests::ConeConeTest, &CollisionTests::ConeCapsuleTest, 
	&CollisionTests::ConePlaneTest, &CollisionTests::ConeMeshTest, &CollisionTests::CapsuleCapsuleTest, &CollisionTests::CapsulePlaneTest,
	&CollisionTests::CapsuleMeshTest, &CollisionTests::PlanePlaneTest, &CollisionTests::PlaneMeshTest, &CollisionTests::MeshMeshTest
	};

const int CollisionTests::helperIndices[7] = { 0, 6, 11, 15, 18, 20, 21 };

int CollisionTests::TestCollision(Collider* a, Collider* b, Contact* contacts)
{
	// These Collider's cannot collide
	// Either 1 of them is turned off, or they do not have matching Collision Masks
	if(!a->enabled || !b->enabled || (a->collisionMask & b->collisionMask == 0)) return 0;

	int _a = (int)a->GetShape(), _b = (int)b->GetShape();

	// Swap collider's for simplicity (a is always 1st parameter sent)
	if(_a > _b)
	{
		Collider* temp = a;
		a = b;
		b = temp;
	}

	// Call appropriate function
	return Tests[helperIndices[_a] + _b](a, b, contacts);
}

bool CollisionTests::AABBTest(AABB a, AABB b)
{
	if(a.max.x < b.min.x) return false;
	if(a.min.x > b.max.x) return false;
	if(a.max.y < b.min.y) return false;
	if(a.min.y > b.max.y) return false;
	if(a.max.z < b.min.z) return false;
	if(a.min.z > b.max.z) return false;
	return true;
}

#pragma region Sphere Collisions
int CollisionTests::SphereSphereTest(Collider* _a, Collider* _b, Contact* contacts)
{
	SphereCollider* s1 = static_cast<SphereCollider*>(_a);
	SphereCollider* s2 = static_cast<SphereCollider*>(_b);

	// Collision true if distance between Sphere centers is less-than sum of their radii 
	// (Squared Distance between Sphere Centers is less-than square of sum of their radii)
	Vector diff = s2->position - s1->position;
	if(diff.SquaredMagnitude() < (s1->radius + s2->radius) * (s1->radius + s2->radius))
	{
		gFloat len = diff.Magnitude();
		Vector normal = diff / len;
		contacts->SetNewContact(s1->attachedBody, s2->attachedBody, 1.0f, 0.0f, normal,  
								s1->position + normal*s1->radius, len - (s1->radius + s2->radius));
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
		contacts->SetNewContact(s->attachedBody, b->attachedBody, 1.0f, 0.0f, normal,
								s->position + normal*s->radius, len - s->radius);
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
	gFloat dist = (s->position - c->p).DotProduct(c->axis); //Δ
	Vector b = c->p + c->axis * dist;

	// Sphere lies between planes defined by ends of Cylinder
	// Collision true if distance between Sphere center and middle of Cylinder is
	// less-than the sum of their radii
	if(dist < c->height)
		return (s->position-b).SquaredMagnitude() < (s->radius+c->radius)*(s->radius*c->radius);

	// Distance along Cylinder axis of Sphere center from Cylinder
	gFloat d = dist > 0 ? dist - c->height : -dist;
	
	// Sphere totally passed planes defined by ends of Cylinder
	if(d >= s->radius) return 0;

	// Distance of....uh....look at the source
	// Distance from (closest point on Sphere to Cylinder Surface) and Sphere center
	gFloat p = Sqrt(s->radius*s->radius - d*d);

	// Collision true if distance between Sphere center and Cylinder axis
	// is less-than sum of Cylinder radius and distance from Cylinder surfance to closest point on Sphere
	return (s->position-b).SquaredMagnitude() < (c->radius + p) * (c->radius + p);
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
	gFloat a = v.DotProduct((c->position - c->tip).Normalized());

	// Sphere totally behind Cone tip
	if(a < 0 && Abs(a) < c->radius)
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
	gFloat e = distCenterToSurface * Cos(c->theta);
	
	// Collision true if shortest distance from Sphere center to Cone surface
	// is less than Sphere radius
	return e < s->radius;
}
int CollisionTests::SphereCapsuleTest(Collider* _a, Collider* _b, Contact* contacts)
{
	SphereCollider* s = static_cast<SphereCollider*>(_a);
	CapsuleCollider* c = static_cast<CapsuleCollider*>(_b);

	gFloat dist = SquaredDistanceOfPointFromLineSegment(s->position, c->p, c->q);
	return dist < (s->radius + c->radius) * (s->radius + c->radius);
}
int CollisionTests::SpherePlaneTest(Collider* _a, Collider* _b, Contact* contacts)
{
	SphereCollider* s = static_cast<SphereCollider*>(_a);
	PlaneCollider* p = static_cast<PlaneCollider*>(_b);

	// Compute distance of Sphere Center from Plane
	// Evaluating plane equation (n.Dot(X) = d) for a point gives signed distance of point to plane
	gFloat dist = s->position.DotProduct(p->normal) - p->d;

	// Collision true if distance from Sphere center to plane is less than Sphere's radius
	return Abs(dist) < s->radius;
}
int CollisionTests::SphereMeshTest(Collider* _a, Collider* _b, Contact* contacts)
{
	SphereCollider* s = static_cast<SphereCollider*>(_a);
	MeshCollider* m = static_cast<MeshCollider*>(_b);

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
	if(code > 6)
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
		contacts->SetNewContact(b1->attachedBody, b2->attachedBody, 1.0f, 0.0f, normal, (p1+p2)/2.0f, -separationDist);
		return 1;
	}

	// Face-something intersection (separating axis is perpendicular to a face)
	// 'a' is reference face (normal is perpendicular to 'a')
	// 'b' is incident face (closest face on other box)

	// New variables to use - set so reference face is 'a' and incidend face is 'b'
	Vector ua[3]={b1->u[0], b1->u[1], b1->u[2]}, ub[3]={b2->u[0], b2->u[1], b2->u[2]},	// axes of each box
		pa=b1->position, pb=b2->position,		// positions of each box
		sa=b1->halfWidths, sb=b2->halfWidths,	// halfwidths of each box ('s' for side)
		normal2 = normal, normalR, absNormalR;	// copy of normal and other useful vars
	if(code > 3)	// Swap them around
	{
		ua[0] = b2->u[0]; ua[1] = b2->u[1]; ua[2] = b2->u[2]; 
		ub[0] = b1->u[0]; ua[1] = b1->u[1]; ua[2] = b1->u[2];
		pa=b2->position; pb=b1->position; 
		sa=b2->halfWidths; sb=b1->halfWidths;
		normal2 = normal * -1.0f;
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
	m11 = R(code1, a1); //ua[code1].DotProduct(ub[a1]);
	m12 = R(code1, a2); //ua[code1].DotProduct(ub[a2]);
	m21 = R(code2, a1); //ua[code2].DotProduct(ub[a1]);
	m22 = R(code2, a2); //ua[code2].DotProduct(ub[a2]);
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
	gFloat det1 = 1.0f/(m11*m22 - m12*m21);
	m11 *= det1;
	m12 *= det1;
	m21 *= det1;
	m22 *= det1;
	int cnum = 0;	// number of Contact points found
	for(unsigned int i = 0; i < n; ++i)
	{
		gFloat k5 = (m22 * (ret[i*2] - c1)) - (m12 * (ret[i*2+1] - c2));
		gFloat k6 = -(m21 * (ret[i*2] - c1)) + (m11 * (ret[i*2+1] - c2));
		for(unsigned int j = 0; j < 3; ++j)
			point[cnum * 3 + j] = center[j] + (k5 * ub[a1][j]) + (k6 * ub[a2][j]);
		depth[cnum] = sa[codeN] - normal2.DotProduct(Vector(*(point+(cnum*3)), *(point+1+(cnum*3)), *(point+2+(cnum*3))));
		
		// Delete non-penetrating points in 'ret' by overwriting next point
		if(depth[cnum] >= 0)
		{
			ret[cnum*2] = ret[i*2];
			ret[cnum*2+1] = ret[i*2+1];
			cnum++;
		}
	}
	if(cnum < 1) return 0;	// THIS SHOULD NEVER HAPPEN
	
	// Create Contacts
	Vector contactPoint;
	gFloat depths=0.0f;
	if(code < 4)
	{
		for(unsigned int i = 0; i < cnum; ++i)
		{
			contactPoint += Vector(pa.x + point[i*3], pa.y + point[i*3+1], pa.z + point[i*3+2]);
			depths += depth[i];
			//contacts->SetNewContact(b1->attachedBody, b2->attachedBody, 0.2f, 0.0f, normal, contactPoint, depth[i]);
			//contacts++;
		}
	}
	else
	{
		for(unsigned int i = 0; i < cnum; ++i)
		{
			contactPoint += Vector(pa.x-normal.x*depth[i] + point[i*3], pa.y-normal.y*depth[i] + point[i*3+1], pa.z-normal.z*depth[i] + point[i*3+2]);
			depths += depth[i];
			//contacts->SetNewContact(b1->attachedBody, b2->attachedBody, 0.2f, 0.0f, normal, contactPoint, depth[i]);
			//contacts++;
		}
	}
	contacts->SetNewContact(b1->attachedBody, b2->attachedBody, 1.0f, 0.0f, normal, contactPoint*(1.0f/(gFloat)cnum), depths/4);
	//return cnum;
	return 1;
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
int CollisionTests::BoxCapsuleTest(Collider* _a, Collider* _b, Contact* contacts)
{
	BoxCollider* b = static_cast<BoxCollider*>(_a);
	CapsuleCollider* c = static_cast<CapsuleCollider*>(_b);

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
	return Abs(d) < r;
}
int CollisionTests::BoxMeshTest(Collider* _a, Collider* _b, Contact* contacts)
{
	BoxCollider* b = static_cast<BoxCollider*>(_a);
	MeshCollider* m = static_cast<MeshCollider*>(_b);

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
int CollisionTests::CylinderCapsuleTest(Collider* _a, Collider* _b, Contact* contacts)
{
	CylinderCollider* c1 = static_cast<CylinderCollider*>(_a);
	CapsuleCollider* c2 = static_cast<CapsuleCollider*>(_b);

	return 0;
}
int CollisionTests::CylinderPlaneTest(Collider* _a, Collider* _b, Contact* contacts)
{
	CylinderCollider* c = static_cast<CylinderCollider*>(_a);
	PlaneCollider* p = static_cast<PlaneCollider*>(_b);

	return 0;
}
int CollisionTests::CylinderMeshTest(Collider* _a, Collider* _b, Contact* contacts)
{
	CylinderCollider* c = static_cast<CylinderCollider*>(_a);
	MeshCollider* m = static_cast<MeshCollider*>(_b);

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
int CollisionTests::ConeCapsuleTest(Collider* _a, Collider* _b, Contact* contacts)
{
	ConeCollider* c1 = static_cast<ConeCollider*>(_a);
	CapsuleCollider* c2 = static_cast<CapsuleCollider*>(_b);

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
int CollisionTests::ConeMeshTest(Collider* _a, Collider* _b, Contact* contacts)
{
	ConeCollider* c = static_cast<ConeCollider*>(_a);
	MeshCollider* m = static_cast<MeshCollider*>(_b);

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
	gFloat min = std::min(std::min(std::min(d1, d2), d3), d4);
	return min < (c1->radius+c2->radius) * (c1->radius+c2->radius);
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
int CollisionTests::CapsuleMeshTest(Collider* _a, Collider* _b, Contact* contacts)
{
	CapsuleCollider* c = static_cast<CapsuleCollider*>(_a);
	MeshCollider* m = static_cast<MeshCollider*>(_b);

	return 0;
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

int CollisionTests::MeshMeshTest(Collider* _a, Collider* _b, Contact* contacts)
{
	MeshCollider* m1 = static_cast<MeshCollider*>(_a);
	MeshCollider* m2 = static_cast<MeshCollider*>(_b);

	return 0;
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
	Vector ab = b - a, ap = p - a, bp = p - b;

	// Length of projection of 'p' onto 'ab'
	gFloat e = ap.DotProduct(ab);

	// 'p' projects outside 'ab'
	if(e < 0) return ap.DotProduct(ap);	// projects behind 'a'
	gFloat f = ab.DotProduct(ab);
	if(e > f) return bp.DotProduct(bp);	// projects passed 'b'

	// 'p' projects onto ab
	return (ap.DotProduct(ap)) - (e * e / f);
}

Vector CollisionTests::ClosestPointonLineSegment(Vector p, Vector a, Vector b)
{
	// Source: 'Real Time Collision Detection' by Christer Ericson, p-128
	Vector ab = b - a;

	// Project 'p' onto 'ab', computing parameterized position d(t) = a + t*(b-a)
	gFloat t = (p - a).DotProduct(ab) / ab.DotProduct(ab);

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