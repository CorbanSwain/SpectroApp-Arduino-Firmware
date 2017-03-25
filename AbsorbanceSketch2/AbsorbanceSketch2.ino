#include "IDGenerator.h"
#include "JSON.h"
#include <Adafruit_BLEMIDI.h>
#include <Adafruit_BLEGatt.h>
#include <Adafruit_BLEEddystone.h>
#include <Adafruit_BLEBattery.h>
#include <Adafruit_ATParser.h>
#include "BluefruitConfig.h"
#include <math.h>
#include "MenuHandler.h"
#include "Logger.h"
#include "SensorController.h"
#include "LedController.h"
#include "AbsorbanceAnalyzer.h"
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DisplayHandler.h"
#include "SchedulerCNS.h"
#include "Button.h"
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
#include <SoftwareSerial.h>
#endif
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#define FACTORYRESET_ENABLE         1
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"

const int
LIGHT_SENSOR = 14,// Analog Pin 0
LED_600NM = 13,// also Adafruit Status LED
BUTTON_A = 9, // OLED Featherwing Button A
BUTTON_B = 6, // OLED Featherwing Button B
BUTTON_C = 5, // OLED Featherwing Button C

buttonCheckInterval = 8,
doubleClickInterval = 160, // must be ~5X longer then the `buttonCheckInterval`
holdInterval = 3000, // bust
longHoldInterval = 8000, // must be longer than the `holdInterval`

ledResreshInterval = 50,
sensorInterval = 50,
analyzerInterval = 50,

displayRefreshInterval = 10,
textSize = 2;

bool verbose[] =
{
	false,	// 0: redButton
	false,	// 1: blueButton
	false,	// 2: OLED display
	false,	// 3: main
	true,	// 4: absorbanceAnalyzer
	false,	// 5: sensor
	true,	// 6: LED
	true	// 7: Menu
};

int intervalArray[] = {
	buttonCheckInterval,
	doubleClickInterval,
	holdInterval,
	longHoldInterval
};

int RED_BUTTON = BUTTON_B; // on left of instrument
int BLUE_BUTTON = BUTTON_C;// on right of instrument
Button redButton = Button(RED_BUTTON, intervalArray, verbose[0]);
Button blueButton = Button(BLUE_BUTTON, intervalArray, verbose[1]);
DisplayHandler display = DisplayHandler(displayRefreshInterval, verbose[2]);
SensorController sensor = SensorController(LIGHT_SENSOR, sensorInterval, verbose[5]);
LedController led = LedController(LED_600NM, verbose[6]);
extern AbsorbanceAnalyzer analyzer = AbsorbanceAnalyzer(led, sensor,
	analyzerInterval,
	verbose[4], 1, 50);
SchedulerCNS loopSchedule = SchedulerCNS(500);
MenuHandler menu = MenuHandler(verbose[7]);
Logger mLog = Logger("main", verbose[3]);
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
int Logger::index = 0;
int Logger::nestLevel = 0;

int counter = 0;
int counter2[2] = { 0 };
int value = 50;
int output = 0;
const int NUM_READINGS = 50;
bool justTookReading = false;

int currentLook = 0;
int numOfReadings = 0;
int numOfRepeats = 3;

// A small helper
void error(const __FlashStringHelper*err) {
	Serial.println(err);
	while (1);
}


void setupBle() {
	delay(500);
	
	/* Initialise the module */
	if (!ble.begin(VERBOSE_MODE))
	{
		error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
	}

	if (FACTORYRESET_ENABLE)
	{
		/* Perform a factory reset to make sure everything is in a known state */
		Serial.println(F("Performing a factory reset: "));
		if (!ble.factoryReset()) 
		{
			error(F("Couldn't factory reset"));
		}
	}

	/* Disable command echo from Bluefruit */
	ble.echo(false);
	ble.verbose(false);  // debug info is a little annoying after this point!

	///* Wait for connection */
	//while (!ble.isConnected()) {
	//	delay(500);
	//}

	// LED Activity command is only supported from 0.6.6
	if (ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION))
	{
		// Change Mode LED Activity
		Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
		ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
	}

	// Set module to DATA mode
	ble.setMode(BLUEFRUIT_MODE_DATA);
}

void setup()
{
	Serial.begin(9600);
	if (verbose[3]) while (!Serial) { delay(500); }

	setupBle();
	mLog.beginFuncLog("setup");
	loopSchedule.setup();
	display.setup();
	redButton.setup();
	blueButton.setup();
	sensor.setup();
	sensor.setResolution(12);
	led.setup();
	loopSchedule.setup();
	analyzer.setup();
	mLog.endFuncLog("setup");

	
	int numSecs = 3;
	display.setTextSize(2);
	if (verbose[3])
	{
		for (int i = 0; i < numSecs; ++i)
		{
			mLog.p("Beginning in: ");
			mLog.pl(numSecs - i);
			display.clear();
			display.print(numSecs - i);
			display.refresh();
			delay(1000);
		}
	}
	display.clear();
	display.refresh();

	menu.printMenu(display);
	menu.wrapAround = true;
	display.section = Q4;
}

void loop()
{
	redButton.getState();
	blueButton.getState();

	switch (redButton.state) {
	case CLICKED:
		menu.highlightNext(display);
		break;
	case DOUBLE_CLICKED:
		menu.highlightPrevious(display);
		break;
	default:
		menu.flashItem(display);
		break;
	}

	
	if (blueButton.state == CLICKED)
	{
		menu.highlightItem(menu.getCurrentItem(), display);

		switch (menu.getCurrentItem())
		{
		case 0: // READ
				
			display.section = menu.infoSec;
			display.clear();
			display.print("\nTakngRedng...");
			display.refreshNow();
			analyzer.takeReading(display);
			justTookReading = true;
			display.section = menu.infoSec;
			display.clear();
			analyzer.recordLastReading();
			justTookReading = false;
			ble.print(analyzer.getReadingJSON(analyzer.getNumReadings() - 1).getString('P'));
			break;

		case 1: // BLANK
			display.section = menu.infoSec;
			display.clear();
			display.print("\nTakngBlank...");
			display.refreshNow();
			analyzer.takeBlank(display);
			justTookReading = true;
			display.section = menu.infoSec;
			display.clear();
			analyzer.recordLastReading();
			justTookReading = false;
			ble.print(analyzer.getReadingJSON(analyzer.getNumReadings() - 1).getString('P'));
			break;

		case 2: // CLEAR
			display.section = menu.infoSec;
			display.clear();
			if (justTookReading)
			{
				justTookReading = false;
				
				display.print("\nDone.");
			}
			else
			{
				display.print("\nAlrdyClred");
			}
			display.refreshNow();
			delay(1000);
			break;

		case 3: // LOOK
			if (justTookReading) 
			{
				analyzer.recordLastReading();
				justTookReading = false;
			}
			display.section = menu.infoSec;
			display.clear();
			display.print("\nVewngLog..");
			display.refreshNow();
			numOfReadings = analyzer.getNumReadings();
			if (numOfReadings == 0)
			{
				display.section = analyzer.sensorSection;
				display.print("No\nRdngsYet!");
				display.refresh();
				while (blueButton.state != HELD)
				{
					blueButton.getState();
				}
			} 
			else
			{
				currentLook = numOfReadings - 1;
				while (blueButton.state != HELD)
				{
					redButton.getState();
					blueButton.getState();

					if (blueButton.state == CLICKED)
					{
						currentLook += 1;
						if (currentLook >= numOfReadings)
							currentLook = 0;
					}
					if (redButton.state == CLICKED)
					{
						currentLook -= 1;
						if (currentLook < 0)
							currentLook = numOfReadings - 1;
					}

					analyzer.printReadingInfo(currentLook, display);
					display.refresh();
				}
			}
			display.section = analyzer.analyzerSection;
			display.clear();
			display.section = analyzer.sensorSection;
			display.clear();
			break;

		default:
			break;
		}
	}
	display.refresh();
}

// TEST CODE
void buttonTestLoop1() {
	switch (redButton.getState()) {
	case UP:
		display.print("U");
		//Serial.print("-"); 
		break;

	case DOWN:
		display.print("D");
		//if (verbose) Serial.println("D");
		break;

	case CLICKED:
		display.print("C!");
		counter += 1;
		Serial.print(counter);
		if (verbose) Serial.println(".\nCLICKED! _____________________");
		//delay(500);
		break;

	case DOUBLE_CLICKED:
		display.print("2C!");
		counter += 1;
		Serial.print(counter);
		if (verbose) Serial.println(".\nDOUBLE_CLICKED! ______________");
		//delay(500);
		break;

	case HELD:
		display.print("H");
		counter += 1;
		Serial.print(counter);
		if (verbose) Serial.println(".\nHELD! ________________________");
		//delay(500);
		break;

	case LONG_HELD:
		display.print("L");
		counter += 1;
		Serial.print(counter);
		if (verbose) Serial.println(".\nLONG HELD! ________________________");
		//delay(500);
		break;
	}

	display.refresh();
}
void buttonTestLoop2()
{
	display.printBarChart(value);

	switch (redButton.getState()) {
	case CLICKED:
		value -= 1;
		break;
	case DOUBLE_CLICKED:
		value -= 5;
		break;
	case HELD:
		value -= 25;
		break;
	case LONG_HELD:
		value -= 100;
		break;
	default:
		break;
	}

	switch (blueButton.getState()) {
	case CLICKED:
		value += 1;
		break;
	case DOUBLE_CLICKED:
		value += 5;
		break;
	case HELD:
		value += 25;
		break;
	case LONG_HELD:
		value += 100;
		break;
	default:
		break;
	}

}
void buttonScreenTestLoop()
{
	redButton.getState();
	blueButton.getState();

	display.section = Q1;
	switch (redButton.state) {
	case CLICKED:
		display.print(0);
		break;
	case DOUBLE_CLICKED:
		display.print(1);
		break;
	case HELD:
		display.clear();
		break;
	case LONG_HELD:
	default:
		break;
	}

	display.section = Q3;
	switch (blueButton.state) {
	case CLICKED:
		display.print(0);
		break;
	case DOUBLE_CLICKED:
		display.print(1);
		break;
	case HELD:
		display.clear();
		break;
	case LONG_HELD:
	default:
		break;
	}
	display.refresh();
}
void buttonSensorLedTestLoop()
{
	redButton.getState();
	blueButton.getState();

	switch (redButton.state)
	{
	case CLICKED:
		++counter2[0];
		break;
	case DOUBLE_CLICKED:
		--counter2[0];
		break;
	case HELD:
		counter2[0] = 0;
		break;
	default:
		break;
	}
	switch (blueButton.state)
	{
	case CLICKED:
		led.switchLed();
		break;
	case DOUBLE_CLICKED:
		led.switchLed();
		break;
	case HELD:
		break;
	default:
		break;
	}
	
	display.section = Q1;
	display.printBarChart(counter2[0], 50, "RED:");

	output = sensor.getOutput();
	
	display.section = Q4;
	display.printBarChart(output, 3140, "SNSR:");
	
	display.section = Q3;
	display.clear();
	display.print("Sensr\nOut:\n");
	if ((output / 1000) == 0) display.print(" ");
	else display.print(output / 1000);
	output = output - 1000 * (output / 1000);
	display.print(output / 100);
	output = output - 100 * (output / 100);
	display.print(output / 10);
	output = output - 10 * (output / 10);
	display.print(output);

	display.refresh();
}
void takeReadingLoop()
{
	display.section = Q1;
	display.clear();
	display.setTextSize(1);
	display.print("Wait-\ning..");
	while (redButton.getState() != CLICKED)
	{
		if (loopSchedule.timeUp()) display.invert(0, 100, 2);
		display.refresh();
	}
	display.clear();
	display.print("TakngRedng");
	display.refresh();

	led.turnOn();
	sensor.beginOutputArray(NUM_READINGS);
	display.section = Q2;
	while (!sensor.arrayComplete)
	{
		sensor.updateArray();
		sensor.printSensorStatus(display, "Collecting");
		display.refresh();
	}
	led.turnOff();

	display.section = Q3;
	if (display.getRemainingChars() < 5) display.clear();
	int average;
	display.print(average = sensor.getOutputArrayAverage());
	mLog.tpl(average);
	display.print("\n");
	display.refresh();
}
void takeReadingLoop2()
{
	display.section = Q1;
	display.clear();
	display.setTextSize(1);
	display.print("Wait-\ning..");
	while (redButton.getState() != CLICKED)
	{
		if (loopSchedule.timeUp()) display.invert(0, 100, 2);
		display.refresh();
	}
	display.clear();
	display.refresh();

	delay(100);

	analyzer.takeReading(display);
}