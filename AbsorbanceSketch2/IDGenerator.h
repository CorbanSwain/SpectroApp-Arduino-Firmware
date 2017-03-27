// IDGenerator.h

#ifndef _IDGENERATOR_h
#define _IDGENERATOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class IDGenerator {
public:
	static String getID(String addOn = "");
};

#endif

