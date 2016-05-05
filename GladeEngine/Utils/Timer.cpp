#include "Timer.h"
#include "Trace.h"

#ifdef _WIN32
#include <Windows.h>
#include <wincon.h>
#endif

using namespace Glade;

gFloat Glade::GetDeltaTime(highResTimeValue t1, highResTimeValue t2)
{
#ifdef _WIN32
	static LARGE_INTEGER frequency;
	static int usePerf = QueryPerformanceFrequency(&frequency);
	return (gFloat)((double)(t2.QuadPart - t1.QuadPart) / (double)frequency.QuadPart);
#else
	return gFloat(0);
#endif
}

highResTimeValue Glade::GetHighResTime(bool& available)
{
	available = false;

#ifdef _WIN32
	LARGE_INTEGER currentTime;
	static LARGE_INTEGER frequency;
	static int usePerf = QueryPerformanceFrequency(&frequency);
	static bool init = false;
	
	if(!init)
	{
		init = true;
		if(usePerf)
			TRACE("Using QueryPerformance for timing\n");
		else
			TRACE("QueryPerformance not available\n");
	}

	if(usePerf)
	{
		available = true;
		QueryPerformanceCounter(&currentTime);
	}
	else
		currentTime.QuadPart = 0;

	return currentTime;
#else
	return highResTimeValue(0);
#endif
}