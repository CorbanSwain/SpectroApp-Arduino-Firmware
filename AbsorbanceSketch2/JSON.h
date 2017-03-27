// JSONParser.h

#ifndef _JSON_h
#define _JSON_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

const int MAX_NUM_KEYS = 10;

class JSON {
	String keys[MAX_NUM_KEYS] = { "" };
	String values[MAX_NUM_KEYS] = { "" };
	
	int numKeyValuePairs = 0;

	static String quotify(String string);
public:
	JSON();
	String getString(char tag);
	void add(String key, String value);
	void add(String key, int value);
	void add(String key, unsigned long value);
	
};

#endif

