#include <iostream>
#include <vector>
#include <sstream>
#include <ctime>
#include <string.h>

struct request_t {
	std::string	method;
	std::string	path;
	std::string	version;
};

std::string	strCutTo(std::string & str, std::string delimiter) {
	std::size_t	found = str.find_first_of(delimiter);
	std::string	word = str.substr(0, found);
	str.erase(0, found + 1);
	return word;
}

request_t	genRequest(std::string str) {
	request_t	request;

	// request.path = "./content/static/index.html";
	request.method = strCutTo(str, " ");
	request.path = strCutTo(str, " ");
	request.version = strCutTo(str, "\r\n");
	return request;
}

int	main(void)
{
	std::string	str = "GET /favicon.ico HTTP/1.1";
	request_t	request = genRequest(str);

	std::cout << request.method << std::endl;
	std::cout << request.path << std::endl;
	std::cout << request.version << std::endl;
}