// 
// 
// 

#include "SchedulerCNS.h"

SchedulerCNS::SchedulerCNS() {
	numIntervals = 1;
	checkIntervals[0] = 50;	
}

SchedulerCNS::SchedulerCNS(int checkIntervals[], 
	int numIntervals) {
	this->numIntervals = numIntervals;
	for (int i = 0; i < numIntervals; ++i) {
		this->checkIntervals[i] = checkIntervals[i];
	}
}

SchedulerCNS::SchedulerCNS(int checkInterval) {
	numIntervals = 1;
	checkIntervals[0] = checkInterval;
}

void SchedulerCNS::setup() {
	for (int i = 0;i < numIntervals; ++i) {
		lastCheckTimes[i] = millis();
	}

	Serial.print("Set up a scheduler with: ");
	Serial.print(numIntervals);
	Serial.println(" intervals.");
}

bool SchedulerCNS::timeUp(int i) {
	if ( ( millis() - lastCheckTimes[i] ) < checkIntervals[i] ) {
		return false;
	}
	else {
		lastCheckTimes[i] = millis();
		return true;
	}
}


void SchedulerCNS::reset(int i) {
	lastCheckTimes[i] = millis();
}

void SchedulerCNS::resetAll() {
	for (int i = 0; i < numIntervals; ++i) {
		lastCheckTimes[i] = millis();
	}
}
