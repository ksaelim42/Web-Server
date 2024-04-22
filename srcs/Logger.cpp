#include "Logger.hpp"

LogLevel Logger::_level = INFO;

void	Logger::setLevel(LogLevel level) {
	Logger::_level = level;
}

bool	Logger::isLog(LogLevel level) {
	if (level <= Logger::_level)
		return true;
	return false;
}
