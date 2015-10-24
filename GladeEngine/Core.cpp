#include "Core.h"

using namespace Glade;

gFloat Glade::sleepEPSILON = ((gFloat)0.0);

void Glade::SetSleepEPSILON(gFloat value)
{
	Glade::sleepEPSILON = value;
}

gFloat Glade::GetSleepEPSILON()
{
	return Glade::sleepEPSILON;
}

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