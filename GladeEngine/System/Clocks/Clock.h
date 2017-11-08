#pragma once
#ifndef GLADE_CLOCK_H
#define GLADE_CLOCK_H
#include "MasterClock.h"
//#include "GameClock.h"

namespace Glade {
class Clock
{
public:
	static Clock& Instance()
	{
		static Clock instance;
		return instance;
	}

	static Glade::GameClock& GameClock()
	{
		return Instance().gc;
	}

	static Glade::MasterClock& MasterClock()
	{
		return Instance().mc;
	}

	void Tick()
	{
		(gc.*gc.tickFunc)();
		mc.Tick();
	}

	Glade::MasterClock mc;
	Glade::GameClock gc;

private:
	Clock() {}
/*
	Clock(Clock const&);
	void operator=(Clock const&);
*/
public:
	Clock(Clock const&) = delete;
	void operator=(Clock const&) = delete;
};
}	// namespace
#endif	// GLADE_CLOCK_H

