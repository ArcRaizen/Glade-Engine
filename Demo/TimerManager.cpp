#include "TimerManager.h"
#include <Windows.h>
#include <mmsystem.h>

unsigned int TimerManager::frameNumber = 0;
unsigned int TimerManager::lastFrameTimestamp = 0;
unsigned int TimerManager::lastFrameDuration = 0;
unsigned long TimerManager::lastFrameClockstamp = 0;
unsigned long TimerManager::lastFrameClockTicks = 0;
bool TimerManager::isPaused = false;
double TimerManager::averageFrameDuration = 0.0;
float TimerManager::fps = 0.0f;

static bool qpcFlag;
static double qpcFrequency;


TimerManager::TimerManager()
{
}


TimerManager::~TimerManager()
{
}

unsigned int SystemTime()
{
	if(qpcFlag)
	{
		static unsigned long long qpcMillisPerTick;
		QueryPerformanceCounter((LARGE_INTEGER*)&qpcMillisPerTick);
		return (unsigned)(qpcMillisPerTick * qpcFrequency);
	}
	else
		return unsigned(timeGetTime());
}

unsigned long SystemClock()
{
	__asm {
		rdtsc;
	}
}

void InitTime()
{
	unsigned long long time;
	qpcFlag = (QueryPerformanceFrequency((LARGE_INTEGER*)&time) > 0);
	if(qpcFlag)
		qpcFrequency = 1000.0 / time;
}

void TimerManager::Init()
{
	InitTime();
	lastFrameTimestamp = SystemTime();
	lastFrameClockstamp = GetClock();
}

unsigned int TimerManager::GetTime()
{
	return SystemTime();
}

unsigned long TimerManager::GetClock()
{
	return SystemClock();
}

void TimerManager::Update()
{
	if(!isPaused)
		frameNumber++;

	unsigned int thisTime = SystemTime();
	lastFrameDuration = thisTime - lastFrameTimestamp;
	lastFrameTimestamp = thisTime;

	unsigned long thisClock = GetClock();
	lastFrameClockTicks = thisClock = lastFrameClockstamp;
	lastFrameClockstamp = thisClock;

	if(frameNumber > 1)
	{
		if(averageFrameDuration <= 0)
			averageFrameDuration = (double)lastFrameDuration;
		else
		{
			averageFrameDuration *= 0.99;
			averageFrameDuration += 0.01 * (double)lastFrameDuration;

			fps = (float)(1000.0 / averageFrameDuration);
		}
	}
}
