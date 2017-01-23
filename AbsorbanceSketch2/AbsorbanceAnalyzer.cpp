//
// AbsorbanceAnalyzer.cpp
//

#include "AbsorbanceAnalyzer.h"

//AbsorbanceAnalyzer::AbsorbanceAnalyzer()
//{
//	numRepeats = 1;
//	samplesPerReading = 20;
//	log = Logger("AbsorbanceAnalyzer", true);
//}

AbsorbanceAnalyzer::AbsorbanceAnalyzer(
	LedController led,
	SensorController sensor,
	int checkInterval, bool verbose,
	int numRepeats, int numSamplesPerReading)
{
	this->led = led;
	this->sensor = sensor;
	this->numRepeats = numRepeats;
	if (numRepeats < 1) numRepeats = 1; // ERROR
	if (numRepeats > MAX_REPEATS) numRepeats = MAX_REPEATS; // ERROR
	samplesPerReading = numSamplesPerReading;
	if (samplesPerReading < 1) samplesPerReading = 1;
	if (samplesPerReading > 500) samplesPerReading = 500;
	measurementSchedule = SchedulerCNS(checkInterval);
	log = Logger("AbsorbanceAnalyzer", verbose);
}

void AbsorbanceAnalyzer::setup()
{
	led.setup();
	sensor.setup();
	measurementSchedule.setup();
}

int AbsorbanceAnalyzer::takeBlank(DisplayHandler &display) {
	log.tpl("Beginning baseline reading...");
	blankValue = takeReading(display, true);
	log.tp("Baseline reading: "); log.pl(blankValue);
	return blankValue;
}

int AbsorbanceAnalyzer::takeReading(DisplayHandler &display, bool isBaseline)
{
	screenSection lastSec = display.section;
	
	log.beginFuncLog("takeReading");
	display.section = analyzerSection;
	display.clear();
	display.print("R#"); display.print(numReadings + 1); display.print("\n");
	display.refresh();

	String readingInfo, readingInfo2;
	int sum = 0;
	lastTimeStamp = millis();
	isLastReadingBlank = isBaseline;
	log.tp("(int)lastTimeStamp: "); log.pl((int)lastTimeStamp);
	for (int i = 0; i < numRepeats; ++i)
	{	
		readingInfo = "    \n" + (String)(i + 1) + "/" + (String)numRepeats + "\n";
		if (log.serialOut)
		{
			readingInfo2 = "Reading " + (String)(i + 1) + "/" + (String)numRepeats;
			log.tpl(readingInfo2);
		}
		
		display.section = sensorSection;
		display.clear();
		led.turnOn();
		sensor.beginOutputArray(samplesPerReading);
		while (!sensor.arrayComplete)
		{
			sensor.updateArray();
			sensor.printSensorStatus(display,readingInfo);
			if (!sensor.arrayComplete) 
				display.invert(1, (i + 1) * 100 / numRepeats);
			display.refresh();
		}
		led.turnOff();
		lastReading[i] = sensor.getOutputArrayAverage();
		sum += lastReading[i];

		display.section = analyzerSection;
		display.print(lastReading[i]); display.print("\n");
	}

	display.section = sensorSection;
	display.clear();
	display.print("\nRedngDone.");
	display.section = lastSec;
	log.endFuncLog("takeReading");
	return sum / numRepeats;
}

void AbsorbanceAnalyzer::recordLastReading()
{
	log.tp("Recording reading ... ");
	for (int i = 0; i < numRepeats; ++i)
	{
		readingLog[index][i] = lastReading[i];
	}
	timeStamps[index] = lastTimeStamp;
	isBlank[index] = isLastReadingBlank;
	numReadings += 1;
	index += 1;
	log.tpl("Done!");
}

int AbsorbanceAnalyzer::getBlank()
{
	return blankValue;
}

int AbsorbanceAnalyzer::getNumReadings()
{
	return numReadings;
}

int AbsorbanceAnalyzer::getNumRepeats()
{
	return numRepeats;
}

const int AbsorbanceAnalyzer::getMaxNumRepeats()
{
	return MAX_REPEATS;
}

void AbsorbanceAnalyzer::getReading(int arrayOut[MAX_REPEATS], int readingIndex)
{
	log.beginFuncLog("getReading(int, int)");

	if (readingIndex < 0) // ERROR
	{
		readingIndex = 0;
		log.tpl("ERROR: readingIndex too small");
	}
	if (readingIndex >= numReadings || readingIndex >= MAX_NUM_READINGS) // ERROR
	{
		readingIndex = MAX_NUM_READINGS - 1;
		log.tpl("ERROR: readingIndex too large");
	}

	log.tp("Getting Reading #"); log.p(readingIndex); log.p(" ... ");
	for (int i = 0; i < numRepeats; ++i)
	{
		arrayOut[i] = readingLog[readingIndex][i];
	}
	log.pl("Done!");
	log.beginFuncLog("getReading(int, int)");
}

void AbsorbanceAnalyzer::getReading(int arrayOut[][MAX_REPEATS], int startIndex, int endIndex)
{
	log.beginFuncLog("getReading(int, int, int)");
	if (startIndex < 0) // ERROR
	{
		startIndex = 0;
		log.tpl("ERROR: startIndex too small");
	}
	if (startIndex >= numReadings || startIndex >= MAX_NUM_READINGS ) // ERROR
	{
		startIndex = MAX_NUM_READINGS - 1;
		log.tpl("ERROR: startIndex too large");
	}
	if (endIndex < 0) // ERROR
	{
		endIndex = 0;
		log.tpl("ERROR: endIndex too small");
	}
	if (endIndex >= numReadings || endIndex >= MAX_NUM_READINGS) // ERROR
	{
		endIndex = MAX_NUM_READINGS - 1;
		log.tpl("ERROR: endIndex too large");
	}

	if (startIndex == endIndex) getReading(arrayOut[0], startIndex);
	else
	{
		log.tp("Getting Readings #'s "); log.p(startIndex); 
		log.p("-"); log.p(endIndex); log.p(" ... ");
		for (int i = startIndex; i <= endIndex; ++i)
		{
			for (int j = 0; j < numRepeats; ++j)
			{
				arrayOut[i - startIndex][j] = readingLog[i][j];
			}
		}
		log.pl("Done!");
	}
	log.endFuncLog("getReading(int, int, int)");
}

int AbsorbanceAnalyzer::printReadingInfo(int index, DisplayHandler &display)
{
	if (index < 0) // ERROR
	{
		index = 0;
	}
	else if (index >= numReadings) // ERROR
	{
		index = numReadings - 1;
		if (index >= MAX_NUM_READINGS)
		{
			index = MAX_NUM_READINGS - 1;
		}
	}

	screenSection lastSec = display.section;

	display.section = sensorSection;
	display.clear();
	display.print("Redng# ");
	display.print(index + 1);
	if (isBlank[index])
	{
		display.print("\nBlank");
	}

	double average = 0;
	double stdDev = 0;
	double normReading[MAX_REPEATS] = { 0 };

	for (int i = 0; i < numRepeats; ++i)
	{
		normReading[i] = (double)readingLog[index][i] / (double)blankValue;
		average += normReading[i];
	}

	average = average / numRepeats;

	double x = 0;
	for (int i = 0; i < numRepeats; ++i)
	{
		x = normReading[i];
		x = average - x;
		x *= x;
		stdDev += x;
	}

	stdDev = stdDev / (double)(numRepeats - 1);
	stdDev = squareRoot(stdDev);

	display.section = analyzerSection;
	display.clear();
	display.print(average, 3);
	display.print("\n +/-\n");
	display.print(stdDev, 3);
	display.print("n="); display.print(numRepeats);
}


void AbsorbanceAnalyzer::printTester(DisplayHandler &display)
{
	log.beginFuncLog("print");
	display.section = FULL;
	display.setTextSize(1);
	display.clear();
	display.print("Abs\nAnlz!");
	display.refresh();
	led.turnOn();
	delay(1000);
	led.turnOff();
	log.endFuncLog("print");
}

/*Returns the square root of n. Note that the function */
double AbsorbanceAnalyzer::squareRoot(double n)
{
	/*We are using n itself as initial approximation
	This can definitely be improved */
	double x = n;
	double y = 1;
	double e = 0.0001; /* e decides the accuracy level*/
	while (x - y > e)
	{
		x = (x + y) / 2;
		y = n / x;
	}
	return x;
}