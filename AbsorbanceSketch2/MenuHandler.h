// MenuHandler.h

#ifndef _MENUHANDLER_h
#define _MENUHANDLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "DisplayHandler.h"
#include "Logger.h"
#include "SchedulerCNS.h"

static const int MAX_NUM_MENU_ITEMS = 4;
const int MENU_FLASH_INTERVAL = 500;

enum readingType {
	BLANK,
	CONTROL,
	STANDARD,
	UNKNOWN,
	KNOWN,
	WILD_TYPE,
	MUTANT,
	CUSTOM,
};

class MenuHandler
{

	int numReadingTypes = 8;
	int head = 0;

	String menuItemShortString[MAX_NUM_MENU_ITEMS] = { "" };

	String menuItemLongString[MAX_NUM_MENU_ITEMS] = { "" };

	SchedulerCNS menuFlasher = SchedulerCNS(MENU_FLASH_INTERVAL);
	Logger log;
	bool invertState[MAX_NUM_MENU_ITEMS] = 
	{ false,false,false,false };
	int currentHighlighted = 0;
	

public:
	bool wrapAround = false;
	screenSection menuSec = Q1;
	screenSection infoSec = Q2;
	bool displayInfo = true;

	MenuHandler(bool verbose = true);
	void setHead(int headIndex);
	void printMenu(DisplayHandler &display);
	void highlightItem(int itemNum, DisplayHandler &display);
	void highlightNext(DisplayHandler &display);
	void highlightPrevious(DisplayHandler &display);
	void flashItem(DisplayHandler &display);
	String getReadingTypeShortString(readingType type);
	String getReadingTypeLongString(readingType type);
	
	readingType getCurrentItem();
	int getCurrentHighlightIndex();
};

#endif

