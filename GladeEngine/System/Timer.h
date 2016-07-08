#ifndef GLADE_TIMER_H
#define GLADE_TIMER_H
#include "../Math/Precision.h"
#include <Windows.h>

namespace Glade { 
class Timer
{
public:
	Timer();

	gFloat TotalTime() const;	// Return total time elapsed since Reset, not including stopped time
	gFloat DeltaTime() const;	// Return time between previous and current frame
	void DeltaTime(gFloat& dt) const;

	void Reset();		// Reset all parameters, as if the timer was brand new
	void Start();		// Resart timer from a Stop/Pause
	void Unpause();		//	Same functionality as Start, just different name
	void Stop();		// Stop timer temporarily
	void Pause();		//	Same functionality as Stop, just different name
	void Tick();		// Update timer now that time has passed

private:
	gFloat secondsPerCount;
	gFloat deltaTime;		// time between previous and current frames

	__int64 baseTime;		// time of last Reset
	__int64 pausedTime;		// total time spent paused since last Reset
	__int64 stopTime;		// time of last stop/pause
	__int64 currTime;		// time of current frame
	__int64 prevTime;		// time of previous frame

	bool isStopped;
};
}	// namespace
#endif GLADE_TIMER_H
