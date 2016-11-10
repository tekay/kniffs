#include "logger.h"

#include <iostream>

int Logger::logLevel = 0;

void Logger::info(std::string msg) {
	if (Logger::logLevel >= INFO) {
		std::cout << "INFO: " << msg << std::endl;
	}
}

void Logger::debug(std::string msg) {
	if (Logger::logLevel >= DEBUG) {
		std::cout << "DEBUG: " << msg << std::endl;
	}
}
