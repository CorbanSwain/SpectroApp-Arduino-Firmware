// LedController.h

#ifndef _LEDCONTROLLER_h
#define _LEDCONTROLLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Logger.h"
#include "SchedulerCNS.h"

class LedController 
{
public:
	enum ledState { OFF, ON };
	ledState state;
	Logger log;
	
private:
	int ledPin;

public:
	LedController(int ledPin = 13, bool verbose = true);
	void setup();
	void switchLed();
	void turnOn();
	void turnOff();
};


#endif

