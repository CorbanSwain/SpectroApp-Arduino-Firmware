// Logger.h

#ifndef _LOGGER_h
#define _LOGGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class Logger
{
	String className;
	
public:
	static int nestLevel;
	static int index;
	bool serialOut = true;

	Logger(String className = "(no class name)", bool verbose = true);
	
	void beginFuncLog(String funcName);
	void endFuncLog(String funcName);
	void beginFuncLog();
	void endFuncLog();

	void t();
	
	void p(String text);
	void p(char text[]);
	void p(int num);
	void p(double num);
	void p(float num);
	void p(bool aBool);

	void tp(String text);
	void tp(char text[]);
	void tp(int num);
	void tp(double num);
	void tp(float num);
	void tp(bool aBool);

	void pl();
	void pl(String text);
	void pl(char text[]);
	void pl(int num);
	void pl(double num );
	void pl(float num);
	void pl(bool aBool);

	void tpl();
	void tpl(String text);
	void tpl(char text[]);
	void tpl(int num);
	void tpl(double num);
	void tpl(float num);
	void tpl(bool aBool);
};

#endif

