// SensorController.h

#ifndef _SENSORCONTROLLER_h
#define _SENSORCONTROLLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "SchedulerCNS.h"
#include "DisplayHandler.h"

static const int MAX_OUTPUT_LENGTH = 100;

class SensorController
{
	int sensorPin;
	unsigned int output = 0;
	int resolution = 12; // 12-bit resolution
	SchedulerCNS sensorScheduler;
	Logger log;
	int numReadings = 0;
	int index = 0;
	int outputArray[MAX_OUTPUT_LENGTH] = { 0 };

public:
	bool arrayComplete = false;

	SensorController(int inputPin = 14, int checkInterval = 50, bool verbose = true);
	void setup();
	int getOutput();
	int getNumReadings();
	void beginOutputArray(int arrayLength);
	void updateArray();
	void getOuputArray(int userArray[]);
	int getOutputArrayAverage();
	void printSensorStatus(DisplayHandler display, String label = "TakngRedng");
	void setResolution(int resolution);
};


#endif

