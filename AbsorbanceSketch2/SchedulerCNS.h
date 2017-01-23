// SchedulerCNS.h

#ifndef _SCHEDULERCNS_h
#define _SCHEDULERCNS_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif



static const int MAX_NUM_SCHEDULER_INTERVALS = 10;

class SchedulerCNS
{
	int numIntervals;
	int checkIntervals[MAX_NUM_SCHEDULER_INTERVALS];
	uint32_t lastCheckTimes[MAX_NUM_SCHEDULER_INTERVALS];

public:
	SchedulerCNS();
	SchedulerCNS(int checkIntervals[], int numIntervals);
	SchedulerCNS(int checkInterval);
	void setup();
	bool timeUp(int i = 0);
	void reset(int i = 0);
	void resetAll();
};

#endif