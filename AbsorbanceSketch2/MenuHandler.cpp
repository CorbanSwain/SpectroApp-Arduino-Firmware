// 
// 
// 

#include "MenuHandler.h"

MenuHandler::MenuHandler(bool verbose)
{
	log = Logger("MenuHandler", verbose);
}

MenuHandler::MenuHandler(int numItems, String shortString[],
	String longString[], bool verbose)
{
	this->numItems = numItems;
	for (int i = 0; i < numItems; ++i)
	{
		menuItemShortString[i] = shortString[i];
		menuItemLongString[i] = longString[i];
	}
	log = Logger("MenuHandler", verbose);
}

void MenuHandler::printMenu(DisplayHandler &display)
{
	log.tp("Printing a Menu ... ");
	bool lastWrapState = display.wrap;
	screenSection lastSec = display.section;
	display.section = menuSec;
	display.wrap = false;
	
	display.clear();
	for (int i = 0; i < numItems; ++i)
	{
		display.print(menuItemShortString[i]);
		display.print("\n");
	}
	display.wrap = lastWrapState;
	display.section = lastSec;

	for (int i = 0; i < MAX_NUM_MENU_ITEMS; ++i)
	{
		invertState[i] = false;
	}

	highlightItem(0, display);

	log.pl("Done.");
}

void MenuHandler::highlightItem(int itemNum, DisplayHandler & display)
{
	if (itemNum < 0) // ERROR
	{
		itemNum = 0;
		log.tpl("ERROR, itemNum too small");
	}
	else if (itemNum >= MAX_NUM_MENU_ITEMS) // ERROR
	{
		itemNum = MAX_NUM_MENU_ITEMS - 1;
		log.tpl("ERROR, itemNum too large");
	}

	log.tp("Highlighting Item #"); log.p(itemNum + 1); log.p(" ... ");
	
	screenSection lastSec = display.section;
	display.section = menuSec;

	if (invertState[currentHighlighted])
	{
		display.invert(currentHighlighted);
		invertState[currentHighlighted] = false;
	}
	
	display.invert(itemNum);
	menuFlasher.reset();
	currentHighlighted = itemNum;
	invertState[currentHighlighted] = true;

	display.section = infoSec;
	display.clear();
	display.print(menuItemLongString[currentHighlighted]);

	display.section = lastSec;
	log.pl("Done.");
}

void MenuHandler::highlightNext(DisplayHandler & display)
{
	if (currentHighlighted == MAX_NUM_MENU_ITEMS - 1)
	{
		if (wrapAround)
		{
			log.tpl("Highlighting up to top...");
			highlightItem(0, display);
		}
		else return;
	}
	else
	{
		log.tpl("Highlighting next...");
		highlightItem(currentHighlighted + 1, display);
	}
}

void MenuHandler::highlightPrevious(DisplayHandler & display)
{
	if (currentHighlighted == 0)
	{
		if (wrapAround)
		{
			log.tpl("Highlighting down to bottom...");
			highlightItem(MAX_NUM_MENU_ITEMS - 1, display);
		}
		else return;
	}
	else
	{
		log.tpl("Highlighting previous...");
		highlightItem(currentHighlighted - 1, display);
	}
}

void MenuHandler::flashItem(DisplayHandler &display)
{
	if (menuFlasher.timeUp())
	{
		screenSection lastSec = display.section;
		display.section = menuSec;

		display.invert(currentHighlighted);
		invertState[currentHighlighted] = 
			!invertState[currentHighlighted];

		display.section = lastSec;
	}
}

int MenuHandler::getCurrentItem()
{
	return currentHighlighted;
}

