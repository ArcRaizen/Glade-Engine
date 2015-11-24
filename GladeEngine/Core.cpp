#include "Core.h"

using namespace Glade;

gFloat Glade::particleSleepEpsilon = ((gFloat)0.3);
gFloat GetParticleSleepEpsilon() { return Glade::particleSleepEpsilon; }
void SetParticleSleepEpsilon(gFloat value) { Glade::particleSleepEpsilon = value; }

#ifdef SLEEP_TEST_ENERGY
	gFloat Glade::sleepEpsilon = ((gFloat)0.0);

	void Glade::SetSleepEpsilon(gFloat value) {	Glade::sleepEpsilon = value; }
	gFloat Glade::GetSleepEpsilon() { return Glade::sleepEpsilon; }
#endif

#ifdef SLEEP_TEST_BOX
	int Glade::sleepStepThreshold = 60;
	gFloat Glade::sleepLinearEpsilon = ((gFloat)0.3);
	gFloat Glade::sleepAngularEpsilon = ((gFloat)0.3);

	int GetSleepStepThreshold() { return Glade::sleepStepThreshold; }
	gFloat GetSleepLinearEpsilon() { return Glade::sleepLinearEpsilon; }
	gFloat GetSleepAngularEpsilon() { return Glade::sleepAngularEpsilon; }
	void SetSleepStepThreshold(int value) { Glade::sleepStepThreshold = value; }
	void SetSleepLinearEpsilon(gFloat value) { Glade::sleepLinearEpsilon = value; }
	void SetSleepAngularEpsilon(gFloat value) { Glade::sleepAngularEpsilon = value; }
#endif

bool Glade::IsNaN(gFloat a)
{
	return a != a;
}

gFloat Glade::AppoximateSquareRoot(gFloat square, gFloat guess, int iterations)
{
	gFloat approx = guess;
	for(; iterations > 0; --iterations)
	{
		approx = guess - ((guess*guess - square) / (2 * guess));
		guess = approx;
	}

	return approx;
}