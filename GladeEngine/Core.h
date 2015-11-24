#ifndef GLADE_CORE_H
#define GLADE_CORE_H

#include <math.h>
#include "Precision.h"
#ifndef GLADE_VECTOR_H
#include "Math\Vector.h"
#endif

namespace Glade {

// ~~~~ Master Engine Defines ~~~~
#define VERLET
#ifndef LEFT_HANDED_COORDS
#define LEFT_HANDED_COORDS
#endif
//#define SLEEP_TEST_ENERGY
#define SLEEP_TEST_BOX
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	extern gFloat particleSleepEpsilon;			// Value for energy under which a Particle will be put to sleep
	gFloat GetParticleSleepEpsilon();			// Get the current particle sleep epsilon value
	void SetParticleSleepEpsilon(gFloat value);	// Set the current particle sleep epsilon value

#ifdef SLEEP_TEST_ENERGY
	extern gFloat sleepEpsilon;			// Value for energy under which a RigidBody will be put to sleep
	
	gFloat GetSleepEpsilon();			// Get the current sleep epsilon value.
	void SetSleepEpsilon(gFloat value);	// Set the current sleep epsilon value.
#endif

#ifdef SLEEP_TEST_BOX
	extern int sleepStepThreshold;		// Number of frames before an object may be put to sleep
	extern gFloat sleepLinearEpsilon;	// Max linear movement before putting an object to sleep
	extern gFloat sleepAngularEpsilon;	// Max angular moement before putting an object to sleep

	// Get/Set sleep test parameters
	int GetSleepStepThreshold();
	gFloat GetSleepLinearEpsilon();
	gFloat GetSleepAngularEpsilon();
	void SetSleepStepThreshold(int value);
	void SetSleepLinearEpsilon(gFloat value);
	void SetSleepAngularEpsilon(gFloat value);
#endif

	bool IsNaN(gFloat a);
	gFloat AppoximateSquareRoot(gFloat square, gFloat guess, int iterations);

	struct AABB
	{
		Vector min, max;

		inline gFloat Width() { return max.x - min.x; }
		inline gFloat Height() { return max.y - min.y; }
		inline gFloat Depth() { return max.z - min.z; }
	};
}
#endif	// GLADE_CORE_H