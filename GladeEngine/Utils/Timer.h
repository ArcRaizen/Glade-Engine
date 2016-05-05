#ifndef GLADE_TIMER_H
#define GLADE_TIMER_H
#include "..\Math\Precision.h"

#ifdef _WIN32
#include <Windows.h>
#endif

namespace Glade
{
#ifdef WIN32
	typedef LARGE_INTEGER highResTimeValue;
#else
	typedef int highResTimeValue;
#endif

	gFloat GetDeltaTime(highResTimeValue t1, highResTimeValue t2);


	// Available indicates if a high-resolution time was available.
	// If not then ignore what this returns
	highResTimeValue GetHighResTime(bool& available);
	
}	// namespace
#endif	// GLADE_TIMER_H