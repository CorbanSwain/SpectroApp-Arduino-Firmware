// 
// 
// 

#include "Button.h"

Button::Button(int buttonPin, int checkIntervals[4], bool verbose) {
	buttonScheduler = SchedulerCNS(checkIntervals, 4);
	for (int i = 0; i < 4; ++i) {
		this->checkIntervals[i] = checkIntervals[i];
	}
	pin = buttonPin;
	log = Logger("Button", verbose);
}

void Button::setup() {
	log.beginFuncLog("setup");
	pinMode(pin, INPUT_PULLUP);
	buttonScheduler.setup();
	log.endFuncLog("setup");
}

Button::buttonPosition Button::getPosition() 
{
	lastPosition = position;
	if (buttonScheduler.timeUp(REFRESH_TIMER)) 
	{
		if (digitalRead(pin) == HIGH) position = NOT_PRESSED;
		else position = PRESSED;
	}

	if (position != lastPosition) 
	{
		upOutputControl = downOutputControl = true;
	}

	return position;
}

buttonState Button::getState()
{
	log.beginFuncLog();
	state = IDLE; // NONE state is assumed
	if (getPosition() == NOT_PRESSED)
	{
		if (lastPosition == NOT_PRESSED)
		{
			if (upOutputControl) 
			{
				log.tpl("not   -> not");
				upOutputControl = false;
			}

			if (waitingOnDoubleClick &&
				buttonScheduler.timeUp(DOUBLE_CLICK_TIMER)) 
			{
				state = CLICKED;
				log.tpl("****************** CLICKED");
				waitingOnDoubleClick = false;
				didJustDouble = false;
			}
		}
		else 
		{
			log.tpl("press -> not");
			if (waitingOnHold && !didJustDouble)
			{	
				buttonScheduler.reset(DOUBLE_CLICK_TIMER);
				waitingOnDoubleClick = true;
			}
			didJustDouble = waitingOnHold = waitingOnLongHold = false;
		}
	}
	else
	{
		if (lastPosition == NOT_PRESSED)
		{
			log.tpl("not   -> press");
			if (waitingOnDoubleClick) 
			{
				if (buttonScheduler.timeUp(DOUBLE_CLICK_TIMER)) 
				{
					state = CLICKED;
					log.tpl("****************** CLICKED");
					waitingOnDoubleClick = true;
					didJustDouble = false;
					buttonScheduler.reset(DOUBLE_CLICK_TIMER);
				}
				else 
				{	
					state = DOUBLE_CLICKED;
					log.tpl("****************** DOUBLE_CLICKED");
					didJustDouble = true;
					waitingOnDoubleClick = false;	
				}
			}
			else 
			{
				buttonScheduler.reset(HOLD_TIMER);
				buttonScheduler.reset(LONG_HOLD_TIMER);
				waitingOnHold = waitingOnLongHold = true;
			}
		}
		else
		{
			if (downOutputControl)
			{
				log.tpl("press -> press");
				downOutputControl = false;
			}
			if (!didJustDouble) {
				if (waitingOnHold && 
					buttonScheduler.timeUp(HOLD_TIMER))
				{
					state = HELD;
					log.tpl("****************** HELD");

					waitingOnHold = false;
				}
				else if (waitingOnLongHold && 
					buttonScheduler.timeUp(LONG_HOLD_TIMER))
				{
					state = LONG_HELD;
					log.tpl("****************** LONG HELD");

					waitingOnLongHold = false;					
				}
			}
		}
	}
	log.endFuncLog();
	return state;
}


buttonState Button::getStateOld() {
	// If the button is *not* currently pressed,
	// switch throuhg all of the possible cases
	// of the most recent state
	if (getPosition() == NOT_PRESSED)
	{
		switch (state) {

			// the state was UP
			//     check if waiting on a double click, if so
			//         check the double-click timer, if expired
			//             set state to CLICKED and *************** 
			//             set to not waiting on double click
			//     otherwise remain on UP
		case UP:
			if (upOutputControl) {
				if (serialOut) Serial.println("not pressed - UP");
				upOutputControl = false;
			}
			if (waitingOnDoubleClick &&
				buttonScheduler.timeUp(DOUBLE_CLICK_TIMER)) {
				state = UP;
				if (serialOut) Serial.println("****************** CLICKED");
				waitingOnDoubleClick = false;
				didJustDouble = false;
				return CLICKED;
			}
			break;

			// the state was DOWN 
			//     if still waiting on a hold and did not just double click
			//         reset the double click timer
			//         begin waiting on double click
			//     continue to default case
		case DOWN:
			if (serialOut) Serial.println("not pressed-DOWN");
			if (waitingOnHold && !didJustDouble) {
				buttonScheduler.reset(DOUBLE_CLICK_TIMER);
				waitingOnDoubleClick = true;
			}
			// NOTE: no `break;` continuing to default case ...

			// the state was CLICKED, DOUBLE_CLICKED, HELD, or LONG_HELD
			//    set state to UP *****************************
			//     no longer did just double click;
			//     no longer waiting on hold
			//     no longer waiting on long hold
		default:
			if (serialOut) Serial.println("not pressed-DEFAULT");
			if (serialOut) Serial.println("******************** UP");
			state = UP;
			upOutputControl = true;
			didJustDouble = waitingOnHold = waitingOnLongHold = false;
		}
	}

	// If the button *is* currently pressed,
	// switch throuhg all of the possible cases
	// of the most recent state
	else {
		switch (state) {
			// the state was UP
			//     if waiting on a double click 
			//          if double click timer has expired
			//             set state to down
			//             but return CLICKED because a click happened before 
			//             it could be registered by the "not pressed - UP" 
			//             control statements
			//             also begin waiting on a double click since a new 
			//             click has begun
			//         otherwise
			//             set state to DOUBLE_CLICKED *****************
			//             record that the button just double clicked
			//             record that not longer waiting on double ckick
			//     otherwise 
			//         set state to DOWN ***************************
			//         reset hold and long hold timers
			//         begin waiting on hold
		case UP:
			if (serialOut) Serial.println("pressed-UP");
			if (waitingOnDoubleClick) {
				if (buttonScheduler.timeUp(DOUBLE_CLICK_TIMER)) {
					state = DOWN;
					if (serialOut) Serial.println("****************** CLICKED");
					waitingOnDoubleClick = true;
					didJustDouble = false;
					buttonScheduler.reset(DOUBLE_CLICK_TIMER);
					return CLICKED;
				}
				else {
					state = DOWN;
					if (serialOut) Serial.println("****************** DOUBLE_CLICKED");
					didJustDouble = true;
					waitingOnDoubleClick = false;
					return DOUBLE_CLICKED;
				}
			}
			else {
				state = DOWN;
				if (serialOut) Serial.println("****************** DOWN");
				downOutputControl = true;
				buttonScheduler.reset(HOLD_TIMER);
				buttonScheduler.reset(LONG_HOLD_TIMER);
				waitingOnHold = waitingOnLongHold = true;
			}
			break;

			// the state was DOWN
			//     if button did not just double click
			//         if waiting on hold and hold time expired
			//             set state to HOLD ******************************
			//             no longer waiting on hold
			//         otherwise
			//             if waiting on long hold and long hold time expired
			//                 set state to LONG_HOLD **********************
			//                 no longer waiting on long hold
			//     otherwise stay on DOWN
		case DOWN:
			if (downOutputControl) {
				if (serialOut) Serial.println("pressed - DOWN");
				downOutputControl = false;
			}
			if (!didJustDouble) {
				if (waitingOnHold &&
					buttonScheduler.timeUp(HOLD_TIMER)) {
					state = DOWN;
					downOutputControl = true;
					waitingOnHold = false;
					if (serialOut) Serial.println("****************** HELD");
					return HELD;
				}
				else if (waitingOnLongHold &&
					buttonScheduler.timeUp(LONG_HOLD_TIMER)) {
					state = DOWN;
					downOutputControl = true;
					waitingOnLongHold = false;
					if (serialOut) Serial.println("****************** LONG HELD");
					return LONG_HELD;
				}
			}
			break;

			// the state was CLICKED, DOUBLE_CLICKED, HELD, or LONG_HELD
			//     set state to DOWN
		default:
			if (serialOut) Serial.println("pressed-DEFAULT");
			if (serialOut) Serial.println("******************* DOWN");
			state = DOWN;
			downOutputControl = true;
		}
	}
	return state;
}
