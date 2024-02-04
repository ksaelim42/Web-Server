#include "Webserv.hpp"

std::string	getStatusText(short int statusCode) {
	switch (statusCode)
	{
	case 200:
		return "OK";
	case 404:
		return "Not Found";
	default:
		return "Undefined";
	}
}


#include <sstream>

std::string	itoa(short int num) {
	std::stringstream	ss;
	ss << num;
	return ss.str();
}

std::string	statusLine(short int statusCode) {
	std::string	httpVer = "HTTP/1.1";;
	std::string	httpStatCode = itoa(statusCode);
	std::string httpStatText = getStatusText(statusCode);
	return httpVer + " " + httpStatCode + " " + httpStatText + CRLF;
}


int	main(void) {
	// Cgi	cgi;

	// cgi.handlerCgi("cgi-bin/test.sh");
	std::cout << statusLine(200);
}