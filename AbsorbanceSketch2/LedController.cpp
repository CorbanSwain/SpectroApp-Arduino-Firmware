// 
// LedController.h
// 

#include "LedController.h"

LedController::LedController(int ledPin, bool verbose) 
{
	this->ledPin = ledPin;
	log = Logger("LedController", verbose);
}

void LedController::setup() 
{
	Serial.println(".\n.\nBeginning setup for LED ...");
	pinMode(ledPin, OUTPUT);
	turnOff();
	Serial.println("Completed LED setup.");
}

void LedController::switchLed()
{
	log.tpl("Switching LED!");
	if (state == ON) turnOff();
	else turnOn();
}

void LedController::turnOn()
{
	log.tpl("LED ON!");
	digitalWrite(ledPin, HIGH);
	state = ON;
	delay(10);
}

void LedController::turnOff()
{
	log.tpl("LED OFF!");
	digitalWrite(ledPin, LOW);
	state = OFF;
	delay(10);
}
