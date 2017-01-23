// 
// 
// 

#include "Logger.h"



Logger::Logger(String className, bool verbose)
{
	this->className = className;
	serialOut = verbose;
}

void Logger::beginFuncLog(String funcName)
{
	pl();
	t();
	nestLevel += 1;
	p(className); p("::"); p(funcName);
	pl(" beginning function...");
}

void Logger::endFuncLog(String funcName)
{
	
	nestLevel -= 1;
	t();
	p(className); p("::"); p(funcName);
	pl(" finished function.");
	pl();
}

void Logger::beginFuncLog()
{
	nestLevel += 1;
}

void Logger::endFuncLog()
{
	nestLevel -= 1;
}

void Logger::t()
{
	for (int i = 0; i < nestLevel; ++i)
	{
		p("   ");
	}
}

void Logger::p(String text)
{
	if (serialOut && Serial)
	{
		Serial.print(text);
		index++;
	}
}

void Logger::p(char text[])
{
	p(String(text));
}

void Logger::p(int num)
{
	p(String(num));
}

void Logger::p(double num)
{
	p(String(num));
}

void Logger::p(float num)
{
	p(String(num));
}

void Logger::p(bool aBool)
{
	if (aBool) p("true");
	else p("false");
}

void Logger::tp(String text)
{
	if (serialOut && Serial)
	{
		t();  Serial.print(text);
		index++;
	}
}

void Logger::tp(char text[])
{
	tp(String(text));
}

void Logger::tp(int num)
{
	tp(String(num));
}

void Logger::tp(double num)
{
	tp(String(num));
}

void Logger::tp(float num)
{
	tp(String(num));
}

void Logger::tp(bool aBool)
{
	if (aBool) tp("true");
	else tp("false");
}

void Logger::pl()
{
	pl(" ");
}

void Logger::pl(String text)
{
	if (serialOut && Serial)
	{
		if ((index % 1000) == 0) {
			Serial.println();
			Serial.print(index); Serial.println(".");
			Serial.println();
		}

		Serial.println(text);
		index++;
	}
}

void Logger::pl(char text[])
{
	pl(String(text));
}

void Logger::pl(int num)
{
	pl(String(num));
}

void Logger::pl(double num)
{
	pl(String(num));
}

void Logger::pl(float num)
{
	pl(String(num));
}

void Logger::pl(bool aBool)
{
	if (aBool) pl("true");
	else pl("false");
}

void Logger::tpl()
{
	pl(" ");
}

void Logger::tpl(String text)
{
	if (serialOut && Serial)
	{
		if ((index % 1000) == 0) 
		{
			Serial.println();
			Serial.print(index); Serial.println(".");
			Serial.println();
		}
		t();  Serial.println(text);
		index++;
	}
}

void Logger::tpl(char text[])
{
	tpl(String(text));
}

void Logger::tpl(int num)
{
	tpl(String(num));
}

void Logger::tpl(double num)
{
	tpl(String(num));
}

void Logger::tpl(float num)
{
	tpl(String(num));
}

void Logger::tpl(bool aBool)
{
	if (aBool) tpl("true");
	else tpl("false");
}