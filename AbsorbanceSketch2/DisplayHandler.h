// DisplayHandler.h

#ifndef _DISPLAYHANDLER_h
#define _DISPLAYHANDLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "SchedulerCNS.h"
#include "Logger.h"
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

enum screenSection { Q1, Q2, Q3, Q4, FULL };
const int NUM_SCREEN_SECTIONS = 5;
const String sectionNames[NUM_SCREEN_SECTIONS] =
{
	"Quad 1",
	"Quad 2",
	"Quad 3",
	"Quad 4",
	"Entire Screen"
};

class DisplayHandler {

	SchedulerCNS displayScheduler;
	Logger log;

	Adafruit_SSD1306 Oled;

	int width = 32;
	int height = 128;

	int secWidth[NUM_SCREEN_SECTIONS] = { 32, 32, 32, 32, 32 };
	int secHeight[NUM_SCREEN_SECTIONS] = { 32, 32, 32, 32, 128 } ;
	int QuadOriginY[NUM_SCREEN_SECTIONS] = { 0 };

	int cursorPosition[NUM_SCREEN_SECTIONS][2] = { { 0 } };
	int remainingChars[NUM_SCREEN_SECTIONS] = { 0 };
	int textSize[NUM_SCREEN_SECTIONS] = { 1, 1, 1, 1, 1 };
	int lastInvertedLine[NUM_SCREEN_SECTIONS] = { 0 };

public:
	bool wrap = true;
	screenSection section = FULL;

	DisplayHandler(int checkInterval = 50, bool verbose = false);
	void setup();
	void refresh();
	void refreshNow();
	void updateQuadrantDimensions();
	void setTextSize(int size);
	void resetCursor();
	void resetAllCursors();
	void clear();
	void printBarChart(int reading, int blankValue = 100,
		String label = "VALUE");
	void invert(int lineNum, int percentOfLine = 100, int numLines = 1);
	void uninvertLastLine();
	void invertCurrentLine(int percent = 100);
	void print(String output);
	void print(int output);
	void print(double output, int numDecimals = 2);
	String resizeToQuadrant(String text);
	int getRemainingChars();

private:
	void printFullBarChart(int reading, int percentReading, int blankValue);
};

#endif

