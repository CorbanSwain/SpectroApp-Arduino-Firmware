// AbsorbanceAnalyzer.h

#ifndef _ABSORBANCEANALYZER_h
#define _ABSORBANCEANALYZER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#include "DisplayHandler.h"
#include "SchedulerCNS.h"
#include "Logger.h"
#include "SensorController.h"
#include "LedController.h"
#include "JSON.h"
#include "IDGenerator.h"
#include "MenuHandler.h"

static const int MAX_NUM_READINGS = 200;
static const int MAX_REPEATS = 10;
static const int MAX_READING_TYPES = 25;

class AbsorbanceAnalyzer {
	
	LedController led = LedController();
	SensorController sensor = SensorController();
	SchedulerCNS measurementSchedule;
	Logger log;


	int numRepeats,
		samplesPerReading;
	int typeCounter[MAX_READING_TYPES] = { 0 };
	int readingLog[MAX_NUM_READINGS][MAX_REPEATS] = { { 0 } };
	unsigned long timeStamps[MAX_NUM_READINGS] = { 0 };
	readingType readingTypes[MAX_NUM_READINGS] = { CUSTOM };
	int readingTypeIndices[MAX_NUM_READINGS] = { 0 };
	String IDLog[MAX_NUM_READINGS] = { "" };
	bool isBlank[MAX_NUM_READINGS] = { false };
	int numReadings = 0;
	int index = 0;

	int lastReadingTypeIndex = 0;
	readingType lastReadingType = CUSTOM;
	int lastReading[MAX_REPEATS] = { 0 };
	unsigned long lastTimeStamp = 0;
	String lastID = "";
	bool isLastReadingBlank = false;

	int blankValue = 2740;
	bool haveRecordedBlank = false;

public:

	screenSection sensorSection = Q3,
		analyzerSection = Q4;

	//AbsorbanceAnalyzer();
	AbsorbanceAnalyzer(
		LedController led,
		SensorController sensor,
		int checkInterval, bool verbose,
		int numRepeats, int numSamplesPerReading);
	void setup();
	int takeReading(DisplayHandler &display, readingType type, bool isRepeat = false);
	void recordLastReading();
	int getBlank();
	int getNumReadings();
	int getNumRepeats();
	const int getMaxNumRepeats();
	void getReading(int arrayOut[MAX_REPEATS], int readingIndex);
	void getReading(int arrayOut[][MAX_REPEATS], int startIntex, int endIndex);

	int printReadingInfo(int readingIndex, DisplayHandler &display);
	String getReadingInfoString(int readingIndex);
	JSON getReadingJSON(int readingIndex);
	void printTester(DisplayHandler &display);

private:
	double squareRoot(double n);
	String getReadingTypeBLEString(readingType type);
};


#endif

