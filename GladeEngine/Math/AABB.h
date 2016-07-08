#ifndef GLADE_AABB_S
#define GLADE_AABB_S

#include "../GladeConfig.h"
#include "Vector.h"

namespace Glade {

struct AABB
{
	AABB() { }
	AABB(Vector c, gFloat hWidth, gFloat hHeight, gFloat hDepth) : center(c) 
	{
		Vector extents(hWidth, hHeight, hDepth);
		minimum = center - extents;
		maximum = center + extents;
		radius = (maximum - minimum).Magnitude() / (gFloat)2.0;
	}
	AABB(Vector min_, Vector max_) : minimum(min_), maximum(max_) 
	{
		center = (minimum + maximum) * gFloat(0.5f);
		radius = (maximum - minimum).Magnitude() / (gFloat)2.0;
	}

	// Test if a point is contained within this AABB
	bool ContainsPoint(Vector p)
	{
		if(p.x < minimum.x) return false;
		if(p.x > maximum.x) return false;
		if(p.y < minimum.y) return false;
		if(p.y > maximum.y) return false;
		if(p.z < minimum.z) return false;
		if(p.z > maximum.z) return false;
		return true;
	}

	void CalcCenter() { center = (minimum + maximum) * gFloat(0.5f); }

	inline gFloat Width() const { return maximum.x - minimum.x; }
	inline gFloat Height() const { return maximum.y - minimum.y; }
	inline gFloat Depth() const { return maximum.z - minimum.z; }

	inline gFloat Halfwidth() const { return (maximum.x - minimum.x) * gFloat(0.5f); }
	inline gFloat Halfheight() const { return (maximum.y - minimum.y) * gFloat(0.5f); }
	inline gFloat Halfdepth() const { return (maximum.z - minimum.z) * gFloat(0.5f); }

	inline Vector GetExtents() const { return Vector(Halfwidth(), Halfheight(), Halfdepth()); }

	inline gFloat GetRadius() const { return radius; }
	inline gFloat GetRadiusSquared() const { return radius * radius; }

	Vector minimum, maximum;
	Vector center;
	gFloat radius;

#ifdef FRUSTUM_CULLING_GEOMETRIC
	int frustumPlane;

	void SetFrustumPlane(int i) { frustumPlane = i; }
	int	 GetFrustumPlane() const { return frustumPlane; }
#endif
};
}	// namespace
#endif	// GLADE_AABB_S