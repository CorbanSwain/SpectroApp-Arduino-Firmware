// 
// 
// 

#include "IDGenerator.h"

String IDGenerator::getID(String addOn)
{
	String timeStr = String(millis());
	String tenDigTimeStr = "";
	for (int i = 0; i < (10 - timeStr.length()); ++i)
	{
		tenDigTimeStr += "0";
	}
	tenDigTimeStr += timeStr;
	unsigned char numDecimals = 10;
	String random = String((double(rand()) / double(RAND_MAX)), numDecimals);
	return addOn + "-" + tenDigTimeStr + "-" + random;
}
