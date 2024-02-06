#include <iostream>
#include <vector>
#include <sstream>
#include <ctime>
#include <string.h>

int	main(void)
{
	std::time_t	currentTime;
	struct tm	*gmTime;
	char		buffer[30];

	std::time(&currentTime);	// get current time.
	gmTime = gmtime(&currentTime);
	// Date: <day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT
	std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %T %Z", gmTime);
}