#ifndef TIMER_H
#define TIMER_H


class TimerManager
{
public:
	TimerManager();
	~TimerManager();

	static void Init();
	static void Update();
	static unsigned int GetTime();
	static unsigned long GetClock();

	static unsigned int frameNumber;
	static unsigned int lastFrameTimestamp;
	static unsigned int lastFrameDuration;
	static unsigned long lastFrameClockstamp;
	static unsigned long lastFrameClockTicks;
	static bool isPaused;

	static double averageFrameDuration;
	static float fps;
};
#endif
