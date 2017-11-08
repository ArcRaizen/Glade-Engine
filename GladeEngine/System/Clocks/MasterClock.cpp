#include "MasterClock.h"
using namespace Glade;

MasterClock::MasterClock() : secondsPerCount(0.0), deltaTime(-1.0), baseTime(0), currTime(0), prevTime(0)
{
	__int64 countsPerSecond;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond);
	secondsPerCount = 1.0 / double(countsPerSecond);
}

double MasterClock::GetTotalTime() const
{
	return (currTime - baseTime) * secondsPerCount;
}

void MasterClock::GetTotalTime(double& tt) const
{
	tt = (currTime - baseTime) * secondsPerCount;
}

double MasterClock::GetDeltaTime() const
{
	return deltaTime;
}

void MasterClock::GetDeltaTime(double& dt) const
{
	dt = deltaTime;
}

void MasterClock::Tick()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	// Calc delta time between previous and current frames
	deltaTime = (currTime - prevTime) * secondsPerCount;
	
	prevTime = currTime;
	if(deltaTime < 0.0)
		deltaTime = 0.0;
}


