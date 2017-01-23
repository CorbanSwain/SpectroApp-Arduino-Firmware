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

class MenuHandler
{
	int numItems = 4;

	String menuItemShortString[MAX_NUM_MENU_ITEMS] =
	{
		"READ",
		"BLANK",
		"CLEAR",
		"LOOK"
	};

	String menuItemLongString[MAX_NUM_MENU_ITEMS] =
	{
		"\nTake\nRedng",
		"\nTake\nBlankRedng",
		"\nClearLast\nRedng",
		"\nLook-up\nRedng"
	};


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
	MenuHandler(int numItems, String shortString[],
		String longString[], bool verbose = true);
	
	void printMenu(DisplayHandler &display);
	void highlightItem(int itemNum, DisplayHandler &display);
	void highlightNext(DisplayHandler &display);
	void highlightPrevious(DisplayHandler &display);
	void flashItem(DisplayHandler &display);
	
	int getCurrentItem();
};

#endif

