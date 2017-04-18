// 
// 
// 

#include "MenuHandler.h"

MenuHandler::MenuHandler(bool verbose)
{
	log = Logger("MenuHandler", verbose);
	setHead(0);
}

void MenuHandler::printMenu(DisplayHandler &display)
{
	log.tp("Printing a Menu ... ");
	bool lastWrapState = display.wrap;
	screenSection lastSec = display.section;
	display.section = menuSec;
	display.wrap = false;
	
	display.clear();
	for (int i = 0; i < MAX_NUM_MENU_ITEMS; ++i)
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

	highlightItem(currentHighlighted, display);

	log.pl("Done.");
}

void MenuHandler::setHead(int headIndex)
{
	if (headIndex < -1) // ERROR
	{
		headIndex = 0;
		log.tpl("ERROR, itemNum too small");
	}
	else if (headIndex > numReadingTypes) // ERROR
	{
		headIndex = numReadingTypes - 1;
		log.tpl("ERROR, itemNum too large");
	}
	else if (headIndex == numReadingTypes)
	{
		headIndex = 0;
	}
	else if (headIndex == -1)
	{
		headIndex = numReadingTypes - 1;
	}
	head = headIndex;
	readingType currentType;
	for (int i = 0; i < MAX_NUM_MENU_ITEMS; ++i)
	{
		currentType = (readingType)((head + i) % numReadingTypes);
		menuItemLongString[i] = getReadingTypeLongString(currentType);
		menuItemShortString[i] = getReadingTypeShortString(currentType);
	}
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

	currentHighlighted = itemNum;
	display.invert(currentHighlighted);
	menuFlasher.reset();
	invertState[currentHighlighted] = true;

	display.section = infoSec;
	display.clear();
	display.print("-----");
	display.print(menuItemLongString[currentHighlighted]);

	display.section = lastSec;
	log.pl("Done.");
}

void MenuHandler::highlightNext(DisplayHandler & display)
{
	if (currentHighlighted == MAX_NUM_MENU_ITEMS - 1)
	{
		setHead(head + 1);
		printMenu(display);
		highlightItem(currentHighlighted, display);
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
		setHead(head - 1);
		printMenu(display);
		highlightItem(currentHighlighted, display);
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

String MenuHandler::getReadingTypeShortString(readingType type)
{
	switch (type) 
	{
	case BLANK:
		return "BLANK";
		break;
	case CONTROL:
		return "CNTRL";
		break;
	case STANDARD:
		return "STNRD";
		break;
	case UNKNOWN:
		return "UNKWN";
		break;
	case KNOWN:
		return "KNOWN";
		break;
	case WILD_TYPE:
		return "W.T.";
		break;
	case MUTANT:
		return "MUTNT";
		break;
	case CUSTOM:
		return "CUSTM";
		break;
	default:
		return "ERR!!";
		break;
	}
}

String MenuHandler::getReadingTypeLongString(readingType type)
{
	String descr;
	switch (type)
	{
	case BLANK:
		return "BlankValue";
		break;
	case CONTROL:
		return "Con-\ntrol";
		break;
	case STANDARD:
		return "Stan-\ndard";
		break;
	case UNKNOWN:
		return "Un-\nknown";
		break;
	case KNOWN:
		return "Known";
		break;
	case WILD_TYPE:
		return "Wild\nType";
		break;
	case MUTANT:
		return "Mut-\nant";
		break;
	case CUSTOM:
		return "Cus-\ntom";
		break;
	default:
		return "DoNotSelct!!!!!";
		break;
	}
}



readingType MenuHandler::getCurrentItem()
{
	return (readingType)((currentHighlighted + head) % numReadingTypes);
}

int MenuHandler::getCurrentHighlightIndex()
{
	return currentHighlighted;
}

