#include "GameClock.h"
using namespace Glade;

GameClock::GameClock() : secondsPerCount(0.0), deltaTime(-1.0), totalPausedTime(0),
				currTime(0), isStopped(false), tickFunc(NULL)
{
	__int64 countsPerSecond;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond);
	secondsPerCount = 1.0 / double(countsPerSecond);
	millisecondsPerCount = secondsPerCount * 1000.;

	// Set initial time for the clock
	QueryPerformanceCounter((LARGE_INTEGER*)&baseTime);
	prevTime = startTime = baseTime;

	tickFunc = &GameClock::Tick;
}

double GameClock::GetTotalTime() const { return currTime - baseTime; }
double GameClock::GetTotalTimeAsSeconds() const { return (currTime - baseTime) * secondsPerCount; }
double GameClock::GetTotalTimeAsMilliseconds() const { return (currTime - baseTime) * millisecondsPerCount; }
double GameClock::GetDeltaTime() const { return deltaTime; }
double GameClock::GetDeltaTimeAsSeconds() const { return deltaTime * secondsPerCount; }
double GameClock::GetDeltaTimeAsMilliseconds() const { return deltaTime * millisecondsPerCount; }
double GameClock::GetCurrentPausedTime() const { if(isStopped) return currTime - stopTime; return 0.; }
double GameClock::GetCurrentPausedTimeAsSeconds() const { if(isStopped) return (currTime - stopTime) * secondsPerCount; return 0.; }
double GameClock::GetCurrentPausedTimeAsMilliseconds() const { if(isStopped) return (currTime - stopTime) * millisecondsPerCount; return 0.; }
double GameClock::GetCurrentUnpausedTime() const { if(!isStopped) return currTime - startTime; return 0.; }
double GameClock::GetCurrentUnpausedTimeAsSeconds() const { if(!isStopped) return (currTime - startTime) * secondsPerCount; return 0.; }
double GameClock::GetCurrentUnpausedTimeAsMilliseconds() const { if(!isStopped) return (currTime - startTime) * millisecondsPerCount; return 0.; }
double GameClock::GetTotalPausedTime() const { return totalPausedTime; }
double GameClock::GetTotalPausedTimeAsSeconds() const { return totalPausedTime * secondsPerCount; }
double GameClock::GetTotalPausedTimeAsMilliseconds() const { return totalPausedTime * millisecondsPerCount; }
double GameClock::GetTotalUnpausedTime() const { return (currTime - baseTime) - totalPausedTime; }
double GameClock::GetTotalUnpausedTimeAsSeconds() const { return ((currTime - baseTime) - totalPausedTime) * secondsPerCount; }
double GameClock::GetTotalUnpausedTimeAsMilliseconds() const { return ((currTime - baseTime) - totalPausedTime) * millisecondsPerCount; }
void GameClock::GetTotalTime(double& tt) const { tt = currTime - baseTime; }
void GameClock::GetTotalTimeAsSeconds(double& tt) const { tt = (currTime - baseTime) * secondsPerCount; }
void GameClock::GetTotalTimeAsMilliseconds(double& tt) const { tt = (currTime - baseTime) * millisecondsPerCount; }
void GameClock::GetDeltaTime(double& dt) const { dt = deltaTime; }
void GameClock::GetDeltaTimeAsSeconds(double& dt) const { dt = deltaTime * secondsPerCount; }
void GameClock::GetDeltaTimeAsMilliseconds(double& dt) const { dt = deltaTime * millisecondsPerCount; }
void GameClock::GetCurrentPausedTime(double& cpt) const { if(isStopped) cpt = currTime - stopTime; cpt = 0.; }
void GameClock::GetCurrentPausedTimeAsSeconds(double& cpt) const { if(isStopped) cpt = (currTime - stopTime) * secondsPerCount; cpt = 0.; }
void GameClock::GetCurrentPausedTimeAsMilliseconds(double& cpt) const { if(isStopped) cpt =  (currTime - stopTime) * millisecondsPerCount; cpt =  0.; }
void GameClock::GetCurrentUnpausedTime(double& cupt) const{ if(!isStopped) cupt = currTime - startTime; cupt = 0.; }
void GameClock::GetCurrentUnpausedTimeAsSeconds(double& cupt) const { if(!isStopped) cupt = (currTime - startTime) * secondsPerCount; cupt = 0.; }
void GameClock::GetCurrentUnpausedTimeAsMilliseconds(double& cupt) const { if(!isStopped) cupt = (currTime - startTime) * millisecondsPerCount; cupt = 0.; }
void GameClock::GetTotalPausedTime(double& tpt) const { tpt = totalPausedTime; }
void GameClock::GetTotalPausedTimeAsSeconds(double& tpt) const { tpt = totalPausedTime * secondsPerCount; }
void GameClock::GetTotalPausedTimeAsMilliseconds(double& tpt) const { tpt = totalPausedTime * millisecondsPerCount; }
void GameClock::GetTotalUnpausedTime(double& tupt) const { tupt = (currTime - baseTime) - totalPausedTime; }
void GameClock::GetTotalUnpausedTimeAsSeconds(double& tupt) const { tupt = ((currTime - baseTime) - totalPausedTime) * secondsPerCount; }
void GameClock::GetTotalUnpausedTimeAsMilliseconds(double& tupt) const { tupt = ((currTime - baseTime) - totalPausedTime) * millisecondsPerCount; }

void GameClock::Reset()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&baseTime);

	prevTime = startTime = baseTime;
	isStopped = false;
	tickFunc = &GameClock::Tick;
}

void GameClock::Start()
{
	if(isStopped)
	{
		__int64 t;
		QueryPerformanceCounter((LARGE_INTEGER*)&t);
		
		startTime = t;
		stopTime = 0;
		isStopped = false;
		tickFunc = &GameClock::Tick;
	}
}

void GameClock::Unpause()
{
	if(isStopped)
	{
		__int64 t;
		QueryPerformanceCounter((LARGE_INTEGER*)&t);
		
		startTime = t;
		stopTime = 0;
		isStopped = false;
		tickFunc = &GameClock::Tick;
	}
}

void GameClock::Stop()
{
	if(!isStopped)
	{
		__int64 t;
		QueryPerformanceCounter((LARGE_INTEGER*)&t);

		stopTime = t;
		isStopped = true;
		deltaTime = 0.0;
		tickFunc = &GameClock::TickWhilePaused;
	}
}

void GameClock::Pause()
{
	if(!isStopped)
	{
		__int64 t;
		QueryPerformanceCounter((LARGE_INTEGER*)&t);

		stopTime = t;
		isStopped = true;
		deltaTime = 0.0;
		tickFunc = &GameClock::TickWhilePaused;
	}
}

void GameClock::TogglePause()
{
	__int64 t;
	QueryPerformanceCounter((LARGE_INTEGER*)&t);

	if(isStopped)
	{		
		startTime = t;
		stopTime = 0;
		isStopped = false;
		tickFunc = &GameClock::Tick;
	}
	else
	{
		stopTime = t;
		isStopped = true;
		deltaTime = 0.0;
		tickFunc = &GameClock::TickWhilePaused;
	}
}

void GameClock::Tick()
{
	// Get current time
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	// Calc delta time between previous and current frames
	deltaTime = (currTime - prevTime);

	// Update previous time for next frame
	prevTime = currTime;

	// Safety
	if(deltaTime < 0.0)
		deltaTime = 0.0;
}

void GameClock::TickWhilePaused()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	totalPausedTime += currTime - prevTime;
	prevTime = currTime;
	return;
}

