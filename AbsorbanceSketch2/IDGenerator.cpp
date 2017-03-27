// 
// 
// 

#include "IDGenerator.h"
//              _    _
//058ed2a3-008d-4ff9-bbd3-fd05b832c56c
//1        9    13   17   21         32
String IDGenerator::getID(String addOn)
{
	unsigned long time1 = millis();
	unsigned long time2 = micros();
	String time1Str = String(time1, (unsigned char)(16));
	String time2Str = String(time2, (unsigned char)(16));
	String eightDigTime1Str = "";
	String eightDigTime2Str = "";
	int len = 8;
	for (int i = 0; i < len; ++i)
	{
		if ((len - i) > time1Str.length())
		{
			eightDigTime1Str += "0";
		} 
		else
		{
			eightDigTime1Str += time1Str.charAt(i - (len - time1Str.length()));
		}
	}
	for (int i = 0; i < len; ++i)
	{
		if ((len - i) > time2Str.length())
		{
			eightDigTime2Str += "0";
		}
		else
		{
			eightDigTime2Str += time2Str.charAt(i - (len - time2Str.length()));
		}
	}

	double r1 = double(rand() % time1) / double(millis());
	double r2 = double(lrand48() % time2) / double(micros());
	r1 -= double(int(r1));
	r2 -= double(int(r2));

	unsigned long ir1 = (unsigned long)(r1 * double(UINT32_MAX));
	unsigned long ir2 = (unsigned long)(r2 * double(UINT32_MAX));

	String sr1 = String(ir1, (unsigned char)(16));
	String sr2 = String(ir2, (unsigned char)(16));

	String sevenDigitRandStr1 = "";
	String sevenDigitRandStr2 = "";

	len = 7;
	for (int i = 0; i < len; ++i)
	{
		if ((len - i) > sr1.length())
		{
			sevenDigitRandStr1 += "0";
		}
		else
		{
			sevenDigitRandStr1 += sr1.charAt(i - (len - sr1.length()));
		}
	}
	for (int i = 0; i < len; ++i)
	{
		if ((len - i) > sr2.length())
		{
			sevenDigitRandStr2 += "0";
		}
		else
		{
			sevenDigitRandStr2 += sr2.charAt(i - (len - sr2.length()));
		}
	}

	String uuid = eightDigTime1Str + eightDigTime2Str + sevenDigitRandStr1 + sevenDigitRandStr2;
	
	return uuid.substring(0, 8) + "-" + uuid.substring(8, 12) + "-4" + uuid.substring(12, 15) + "-b" + uuid.substring(15, 18) + "-" + uuid.substring(18);
}
