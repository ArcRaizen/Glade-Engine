#ifndef GLADE_RAY_S
#define GLADE_RAY_S
#include "Vector.h"

namespace Glade{
struct Ray
{
	Ray() { }
	Ray(Vector o, Vector d) : origin(o), dir(d), len(1000.f) { }
	Ray(Vector o, Vector d, gFloat l) : origin(o), dir(d), len(l) { }

	Vector GetEndPoint() { return origin + dir*len; }

	Vector origin;
	Vector dir;
	gFloat len;
};
}	// namespace
#endif	// GLADE_RAY_S