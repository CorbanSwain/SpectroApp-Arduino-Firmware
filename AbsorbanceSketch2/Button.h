// Button.h

#ifndef _BUTTON_h
#define _BUTTON_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "SchedulerCNS.h"
#include "Logger.h"

enum buttonState {
	
	// IDLE is  the default state of the button
	IDLE,
	
	// DEPRECIATED
	// UP is whenever the button is not pressed,
	// except for just after a CLICK
	UP,

	// DEPRECIATED
	// DOWN is whenever the button is pressed,
	// except for just after a DOUBLE CLICK, HOLD, or LONG HOLD
	DOWN,

	// CLICKED is registerd only when these conditions are satisfied: 
	// -> the button is not pressed
	// -> the button was in the UP state
	// -> the button is waiting on a double click,
	// -> the double click timer has expired,
	// -> the button is still waiting on a hold 
	// -> the button did not just double click
	CLICKED,

	// DOUBLE_CLICKED is registered only when these conditions are satisfied:
	// -> the button is pressed
	// -> the button was just in the UP state
	// -> the button is waiting on a double click
	// -> the double click timer has not expired
	DOUBLE_CLICKED,

	// HELD is registered only when
	// -> the button is pressed
	// -> the button was just in the DOWN state
	// -> the button is waiting on a hold and long hold
	// -> the hold timer has expired
	// -> the button did not just double click
	HELD = 10,

	// LONG_HELD is registered when
	// -> the button is pressed
	// -> the button was just in the DOWN or HOLD state
	// -> the button is not waiting on a hold
	// -> the button is waiting on a long hold
	// -> the long hold timer has expired
	// -> the button did not just double click
	LONG_HELD,
};


class Button {
	
	enum buttonPosition { NOT_PRESSED, PRESSED };
	
	// index IDs for chosing which interval to check, 
	// the time intervals are defined as follows
	enum intervalID {
		// how often to refresh the button state
		REFRESH_TIMER, 
		
		// 1: second press following a click will not be regarded as a
		// double click if it takes longer than this interval
		DOUBLE_CLICK_TIMER, 
		
		// 2: length of time for a single button 
		// press to register as a hold
		HOLD_TIMER,

		// 3: time for a press to to register as a long hold
		LONG_HOLD_TIMER
	};
	
	buttonPosition position = NOT_PRESSED;
	
	SchedulerCNS buttonScheduler;
	Logger log;
	
	int checkIntervals[4] = { 0,0,0,0 };

public:
	buttonState state = UP;
	int pin;
	bool serialOut;
	bool upOutputControl = true;
	bool downOutputControl = true;
	
	
	buttonPosition lastPosition = NOT_PRESSED;
	bool waitingOnDoubleClick = false;
	bool didJustDouble = false;
	bool waitingOnHold = false;
	bool waitingOnLongHold = false;

	Button(int buttonPin, int checkIntervals[4], bool verbose = true);
	void setup();
	buttonPosition getPosition();
	buttonState getState();
	buttonState getStateOld();
};

#endif

