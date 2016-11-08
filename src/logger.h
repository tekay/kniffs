#ifndef LOGGER_H__
#define LOGGER_H__

#include <string>

class Logger {
	public:
	/*
		0 = no output
		1 = info
			display everything related to the game logic
		2 = debug
			more output
	*/
		static int logLevel;
		static const int INFO = 1;
		static const int DEBUG = 2;

		static void info(std::string msg);
		static void debug(std::string msg);

	private:
		Logger();
};

#endif
