//
//  DisplayHandler.h
//

#include "DisplayHandler.h"


DisplayHandler::DisplayHandler(int checkInterval, bool verbose):
	Oled(Adafruit_SSD1306()),
	displayScheduler(SchedulerCNS(checkInterval)),
	log(Logger("DisplayHandler",verbose))
{
	;
}

void DisplayHandler::setup() {
	Serial.println(".\n.\nStarting OLED Setup:");

	Serial.print("Powering on OLED FeatherWing...");
	// initialize with the I2C addr 0x3C (for the 128x32),
	// and generate high voltage from the 3.3v line internally
	Oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
	Serial.println("OLED Powered on.");

	for (int i = 0; i < NUM_SCREEN_SECTIONS; ++i)
	{
		textSize[i] = 1;
		secWidth[i] = 32;
	}

	// Show splashscreen...
	Oled.clearDisplay();
	Oled.setTextSize(2);
	Oled.setCursor(0, 0);
	Oled.setTextColor(WHITE);
	Oled.println("Spectrum\nPerception");
	Oled.display();
	delay(2000); // ...for 2 seconds

	Oled.setTextSize(1);
	Oled.setRotation(3); // rotate screen vertically
	width = Oled.width();
	height = Oled.height();
	updateQuadrantDimensions();
	resetAllCursors();

	Serial.println("Completed OLED Setup.");
}

void DisplayHandler::refresh()
{
	if (displayScheduler.timeUp())
	{
		log.tpl("Refreshing display.");
		Oled.display();
	}
}

void DisplayHandler::refreshNow()
{
		Oled.display();
}

void DisplayHandler::updateQuadrantDimensions()
{
	log.tpl("Updating quadrant dimensions.");
	int qHeight = height / 4;
	int multiplier = 0;
	for (int i = Q1; i <= Q4; ++i)
	{
		secY_Origin[i] = multiplier * qHeight;
		secHeight[i] = qHeight;
		secWidth[i] = width;
		++multiplier;
	}
	secWidth[FULL] = width;
	secHeight[FULL] = height;
	secY_Origin[FULL] = 0;
}

void DisplayHandler::setTextSize(int size)
{
	textSize[section] = size;
}

void DisplayHandler::resetCursor()
{
	log.tp("Resetting cursor for: "); log.pl(secNames[section]);
	cursorPosition[section][0] = 0;
	cursorPosition[section][1] = secY_Origin[section];
}

void DisplayHandler::resetAllCursors()
{
	log.beginFuncLog("resetAllCursors");
	screenSection originalSection = section;
	for (int i = 0; i < 4; ++i)
	{
		section = static_cast<screenSection>(i);
		resetCursor();
	}
	section = originalSection;
	log.endFuncLog("resetAllCursors");
}

void DisplayHandler::clear()
{
	log.tp("Clearing: "); log.pl(secNames[section]);
	if (section == FULL) Oled.clearDisplay();
	else
	{
		Oled.fillRect(0, secY_Origin[section],
			secWidth[section], secHeight[section], BLACK);
	}
	resetCursor();
}

void DisplayHandler::printBarChart(int reading, int blankValue,
	String label) 
{
	int percentReading = reading * 100 / blankValue;
	log.tp("To OLED Barchart -> "); log.p(percentReading); log.pl(" %");

	if (section == FULL) printFullBarChart(reading, percentReading, blankValue);
	else
	{
		clear();
		wrap = false;
		print(label); print("\n");
		wrap = true;
		print(percentReading, section); print(" %");

		int secBottom = secY_Origin[section] + secHeight[section] - 1;
		
		int tickSpacing = secWidth[section] / 6;
		int bigTickSpacing = tickSpacing * 3;


		for (int i = 0; i < secWidth[section]; i += tickSpacing)
		{
			Oled.drawFastVLine(i, secBottom - 5, 2, WHITE);
		}

		for (int i = 0; i < secWidth[section]; i += bigTickSpacing)
		{
			Oled.drawFastVLine(i, secBottom - 6, 3, WHITE);
		}

		int maxWidth = tickSpacing * 6 + 1;
		int barWidth = reading * maxWidth / blankValue;
		if (barWidth > maxWidth) barWidth = maxWidth;
		if (barWidth < 0) barWidth = 0;
		int barHeight = 10;

		Oled.fillRect(0, secBottom - barHeight - 3, barWidth, barHeight, INVERSE);
	}
}

void DisplayHandler::invert(int lineNum, int percentOfLine, int numLines)
{
	log.tp("Inverting Display! ... ");
	int lineHeight = 8 * textSize[section];
	int x0 = 0;
	int y0 = secY_Origin[section] + lineHeight*lineNum;
	int bottomOfSection = secY_Origin[section] + secHeight[section] - 1;
	if (y0 > bottomOfSection) return;
	int width = percentOfLine * secWidth[section] / 100;
	int height = lineHeight;
	if (numLines > 1)
	{
		int maxHeight = bottomOfSection - y0 + 1;
		height = lineHeight * numLines;
		if (height > maxHeight) height = maxHeight;
	}
	lastInvertedLine[section] = lineNum;
	Oled.fillRect(x0, y0, width, height, INVERSE);
	log.tp("Done.");
}

void DisplayHandler::uninvertLastLine()
{
	invert(lastInvertedLine[section]);
}

void DisplayHandler::invertCurrentLine(int percent)
{
	int cursorX = cursorPosition[section][0];
	int cursorY = cursorPosition[section][1];
	int lineHeight = 8 * textSize[section];
	int currentLine = (cursorY - secY_Origin[section]) / lineHeight;
	if (cursorX == 0 && currentLine != 0) currentLine -= 1;
	invert(currentLine, percent);
}

void DisplayHandler::print(String output)
{
	log.beginFuncLog("print");
	//if (displayScheduler.timeUp())
	//{
	int& cursorX = cursorPosition[section][0];
	int& cursorY = cursorPosition[section][1];

	Oled.setCursor(cursorX, cursorY);
	Oled.setTextSize(textSize[section]);
	Oled.setTextWrap(wrap);
	if (section == FULL)
		Oled.print(output);
	else
		Oled.print(resizeToQuadrant(output));
	
	log.tpl("Printing to screen");

	cursorX = Oled.getCursorX();
	cursorY = Oled.getCursorY();
	//}
	log.endFuncLog("print");
}


void DisplayHandler::print(int output)
{
	print(String(output));
}

void DisplayHandler::print(double output, int numDecimals)
{
	unsigned char charNumDecimals = numDecimals;
	String outputStr = String(output, charNumDecimals);
	print(outputStr);
}

String DisplayHandler::resizeToQuadrant(String text)
{
	log.beginFuncLog("resizeToQuadrant");
	int textLength = text.length();

	if (textLength < 1)
	{
		log.tpl("Passed null string.");
		log.endFuncLog("resizeToQuadrant");
		return String("");
	}

	getRemainingChars();
	if (remainingChars[section] < textLength)
	{
		log.tpl("String too long.");
		String reducedText;
		if (remainingChars[section] < 1)
		{
			reducedText = "";
		}
		else
		{
			reducedText = text.substring(0, remainingChars[section]);
		}
		log.tp("Text reduced to: `"); log.p(reducedText); log.pl("`");
		log.endFuncLog("resizeToQuadrant");
		return reducedText;
	}
	else
	{
		log.tpl("String not too long.");
		log.endFuncLog("resizeToQuadrant");
		return text;
	}
}

int DisplayHandler::getRemainingChars()
{
	log.beginFuncLog("getRemainingChars");

	log.tp("Getting remain chars for: "); log.pl(secNames[section]);
	int16_t cursorX = cursorPosition[section][0];
	int16_t cursorY = cursorPosition[section][1];
	int bottomOfSection = secY_Origin[section] + secHeight[section] - 1;
	int lineHeight = 8 * textSize[section];
	int charWidth = 6 * textSize[section];
	int remainingLines = (bottomOfSection - cursorY + 1) / lineHeight;
	log.tp("remainingLines:  "); log.pl(remainingLines);
	int remainingCharsOnLine = (secWidth[section] - cursorX) / charWidth;
	log.tp("remaingCharsOnLine:  "); log.pl(remainingCharsOnLine);
	int charsPerLine = secWidth[section] / charWidth;
	int totalRemainingChars = 0;
	if (remainingLines > 0)
	{
		totalRemainingChars = remainingCharsOnLine + charsPerLine * (remainingLines - 1);
	}
	log.tp("totalRemaingChars:  "); log.pl(totalRemainingChars);
	remainingChars[section] = totalRemainingChars;
	
	log.endFuncLog("getRemainingChars");
	return remainingChars[section];
}

void DisplayHandler::printFullBarChart(int reading, int percentReading,
	int blankValue)
{
	int maxBarHeight = (height - 8);
	int barHeight = reading * maxBarHeight/ blankValue;
	if (barHeight > maxBarHeight)  barHeight = maxBarHeight;
	if (barHeight < 0) barHeight = 0;

	clear();
	// Display bar graph on OLED as a filled rectangle
	Oled.fillRect(0, (128 - barHeight), 32, barHeight, INVERSE);
	resetCursor();
	// Also show the percent value
	print(percentReading); print(" %"); 
	// call `refresh()` to show
}
