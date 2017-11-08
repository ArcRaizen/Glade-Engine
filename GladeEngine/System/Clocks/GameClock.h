#ifndef GLADE_GAME_CLOCK_H
#define GLADE_GAME_CLOCK_H
#include "../../Math/Precision.h"
#include <Windows.h>

namespace Glade { 
class Clock;
class GameClock
{
private:
	GameClock();

public:
	double GetTotalTime() const;			// Return total time elapsed since Reset, not including stopped time
	double GetTotalTimeAsSeconds() const;
	double GetTotalTimeAsMilliseconds() const;
	double GetDeltaTime() const;			// Return time between previous and current frame
	double GetDeltaTimeAsSeconds() const;
	double GetDeltaTimeAsMilliseconds() const;	
	double GetCurrentPausedTime() const;	// Return time of current pause
	double GetCurrentPausedTimeAsSeconds() const;	
	double GetCurrentPausedTimeAsMilliseconds() const;	
	double GetCurrentUnpausedTime() const;	// Return total time since end of last pause
	double GetCurrentUnpausedTimeAsSeconds() const;
	double GetCurrentUnpausedTimeAsMilliseconds() const;
	double GetTotalPausedTime() const;		// Return total time paused since Reset
	double GetTotalPausedTimeAsSeconds() const;
	double GetTotalPausedTimeAsMilliseconds() const;
	double GetTotalUnpausedTime() const;	// Return total time not paused since Reset		
	double GetTotalUnpausedTimeAsSeconds() const;
	double GetTotalUnpausedTimeAsMilliseconds() const;
	void GetTotalTime(double& tt) const;
	void GetTotalTimeAsSeconds(double& tt) const;
	void GetTotalTimeAsMilliseconds(double& tt) const;
	void GetDeltaTime(double& dt) const;
	void GetDeltaTimeAsSeconds(double& dt) const;
	void GetDeltaTimeAsMilliseconds(double& dt) const;	
	void GetCurrentPausedTime(double& cpt) const;
	void GetCurrentPausedTimeAsSeconds(double& cpt) const;	
	void GetCurrentPausedTimeAsMilliseconds(double& cpt) const;	
	void GetCurrentUnpausedTime(double& cupt) const;
	void GetCurrentUnpausedTimeAsSeconds(double& cupt) const;
	void GetCurrentUnpausedTimeAsMilliseconds(double& cupt) const;
	void GetTotalPausedTime(double& tpt) const;
	void GetTotalPausedTimeAsSeconds(double& tpt) const;
	void GetTotalPausedTimeAsMilliseconds(double& tpt) const;
	void GetTotalUnpausedTime(double& tupt) const;
	void GetTotalUnpausedTimeAsSeconds(double& tupt) const;
	void GetTotalUnpausedTimeAsMilliseconds(double& tupt) const;

	void Reset();		// Reset all parameters, as if the timer was brand new
	void Start();		// Resart timer from a Stop/Pause
	void Unpause();		// Same functionality as Start, just different name
	void Stop();		// Stop timer temporarily
	void Pause();		// Same functionality as Stop, just different name
	void TogglePause();
	void Tick();		// Update timer now that time has passed
	void TickWhilePaused();


	double secondsPerCount;
	double millisecondsPerCount;
	double deltaTime;			// time between previous and current frames
	double totalUnpausedTime;	// total time spent not paused since last Reset
	double totalPausedTime;		// total time spent paused since last Reset

	__int64 baseTime;		// time of last Reset
	__int64 startTime;		// time of last start/unpause
	__int64 stopTime;		// time of last stop/pause
	__int64 currTime;		// time of current frame
	__int64 prevTime;		// time of previous frame

	bool isStopped;
	void (GameClock::*tickFunc)();

	friend class Clock;
};
}	// namespace
#endif GLADE_GAME_CLOCK_H
