#include "Timer.h"
using namespace Glade;

Timer::Timer() : secondsPerCount(0.0f), deltaTime(-1.0f), baseTime(0), pausedTime(0),
				currTime(0), prevTime(0), isStopped(false)
{
	__int64 countsPerSecond;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond);
	secondsPerCount = gFloat(1.0f) / gFloat(countsPerSecond);
}

gFloat Timer::TotalTime() const
{
	if(isStopped)
		return (gFloat)(((stopTime - pausedTime) - baseTime) * secondsPerCount);

	return (gFloat)(((currTime - pausedTime) - baseTime) * secondsPerCount);
}

gFloat Timer::DeltaTime() const
{
	return deltaTime;
}

void Timer::DeltaTime(gFloat& dt) const
{
	dt = deltaTime;
}

void Timer::Reset()
{
	__int64 t;
	QueryPerformanceCounter((LARGE_INTEGER*)&t);

	baseTime = t;
	prevTime = t;
	stopTime = 0;
	isStopped = false;
}

void Timer::Start()
{
	if(isStopped)
	{
		__int64 t;
		QueryPerformanceCounter((LARGE_INTEGER*)&t);
		
		// Accumulate time elapsed since previous stop/pause
		pausedTime += (t - stopTime);
		prevTime = t;
		stopTime = 0;
		isStopped = false;
	}
}

void Timer::Unpause()
{
	if(isStopped)
	{
		__int64 t;
		QueryPerformanceCounter((LARGE_INTEGER*)&t);
		
		// Accumulate time elapsed since previous stop/pause
		pausedTime += (t - stopTime);
		prevTime = t;
		stopTime = 0;
		isStopped = false;
	}
}

void Timer::Stop()
{
	if(!isStopped)
	{
		__int64 t;
		QueryPerformanceCounter((LARGE_INTEGER*)&t);

		stopTime = t;
		isStopped = true;
	}
}

void Timer::Pause()
{
	if(!isStopped)
	{
		__int64 t;
		QueryPerformanceCounter((LARGE_INTEGER*)&t);

		stopTime = t;
		isStopped = true;
	}
}

void Timer::Tick()
{
	if(isStopped)
	{
		deltaTime = gFloat(0.0f);
		return;
	}

	__int64 t;
	QueryPerformanceCounter((LARGE_INTEGER*)&t);
	currTime = t;

	// Calc delta time between previous and current frames
	deltaTime = (currTime - prevTime) * secondsPerCount;

	prevTime = currTime;

	if(deltaTime < gFloat(0.0f))
		deltaTime = gFloat(0.0f);
}

