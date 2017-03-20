// 
// 
// 

#include "IDGenerator.h"

String IDGenerator::getID(String addOn)
{
	uint32_t time = millis();
	String timeStr = String(time);
	String tenDigTimeStr = "";
	for (int i = 0; i < (10 - timeStr.length()); ++i)
	{
		tenDigTimeStr += "0";
	}
	tenDigTimeStr += timeStr;
	unsigned char numDecimals = 10;
	String random = String((double(rand() % time) / double(millis())), numDecimals);
	random.remove(0, 2);
	return addOn + "-" + tenDigTimeStr + "-" + random;
}
