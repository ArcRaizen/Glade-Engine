#pragma once
#ifndef GLADE_MASTER_CLOCK_H
#define GLADE_MASTER_CLOCK_H
#include "GameClock.h"

namespace Glade { 
class MasterClock
{
private:
	MasterClock();

public:
	double GetTotalTime() const;
	void GetTotalTime(double& tt) const;
	double GetDeltaTime() const;
	void GetDeltaTime(double& dt) const;

	void Start();
	void Tick();

	double secondsPerCount;
	double deltaTime;

	__int64 baseTime;
	__int64 currTime;
	__int64 prevTime;

	friend class Clock;
};
}	// namespace
#endif	// GLADE_MASTER_CLOCK_H
