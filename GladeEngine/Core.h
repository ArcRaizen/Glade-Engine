#ifndef GLADE_CORE_H
#define GLADE_CORE_H

#include <math.h>
#include "Precision.h"
#ifndef GLADE_VECTOR_H
#include "Math\Vector.h"
#endif

namespace Glade {

	// Value for energy under which a body will be put to sleep.
	extern gFloat sleepEPSILON;

	// Get/Set the current sleep EPSILON value.
	gFloat GetSleepEPSILON();
	void SetSleepEPSILON(gFloat value);

	bool IsNaN(gFloat a);
	gFloat AppoximateSquareRoot(gFloat square, gFloat guess, int iterations);

	struct AABB
	{
		Vector min;
		Vector max;
	};
}
#endif	// GLADE_CORE_H