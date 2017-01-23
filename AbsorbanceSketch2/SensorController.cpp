// 
// SensorController.cpp
// 

#include "SensorController.h"

SensorController::SensorController(int inputPin, int checkInterval, bool verbose)
{
	this->sensorPin = inputPin;
	sensorScheduler = SchedulerCNS(checkInterval);
	log = Logger("SensorController",verbose);
}

void SensorController::setup()
{
	Serial.println(".\n.\nBeginning sensor setup ...");
	pinMode(sensorPin, OUTPUT);
	analogReadResolution(resolution);
	output = analogRead(sensorPin);
	sensorScheduler.setup();
	Serial.println("Completed sensor setup.");
}

int SensorController::getOutput()
{
	if (sensorScheduler.timeUp())
	{
		output = analogRead(sensorPin);
		log.tp("Getting sensor output: "); log.pl((int)output);
	}
	return output;
}

int SensorController::getNumReadings()
{
	return numReadings;
}

void SensorController::beginOutputArray(int arrayLength)
{
	arrayComplete = false;
	numReadings = arrayLength;
	if (numReadings > 100) numReadings = 100;
	if (numReadings < 1) numReadings = 1;
	index = 0;
	updateArray();
}

void SensorController::updateArray()
{
	if (!arrayComplete && sensorScheduler.timeUp())
	{
		outputArray[index] = analogRead(sensorPin);
		index += 1;
		if (numReadings == index)
		{
			arrayComplete = true;
		}
	}
}

void SensorController::getOuputArray(int userArray[])
{
	if (arrayComplete)
	{
		for (int i = 0; i < numReadings; ++i)
		{
			userArray[i] = outputArray[i];
		}
	}
}

int SensorController::getOutputArrayAverage()
{
	int average = 0;
	for(int i = 0; i < numReadings; ++i) 
	{
		average += outputArray[i];
	}
	average /= numReadings;
	return average;
}

void SensorController::printSensorStatus(DisplayHandler display, String label)
{
	display.clear();
	if (!arrayComplete)
	{
		display.print(label);
		display.print(index); display.print("/"); display.print(numReadings);
		display.invertCurrentLine(index * 100 / numReadings);
	}
	else
	{
		display.print("RedngDone.");
	}
}

void SensorController::setResolution(int res)
{
	resolution = res;
	analogReadResolution(resolution);
} 

