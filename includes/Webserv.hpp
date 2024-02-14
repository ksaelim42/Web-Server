#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# define HTTP_VERS		"HTTP/1.1"
# define PROGRAM_NAME	"mpm/1.0"
# define CGI_VERS		"CGI/1.0"
# define RED			"\e[0;31m"
# define GREEN			"\e[0;32m"
# define RESET			"\e[0m"
# define PORT			8080
# define CRLF			"\r\n"

#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fstream>
#include <exception>
#include <vector>
#include <ctime>
#include <new>

// for test
struct request_t {
	std::string	method;
	std::string	path;
	std::string	version;
	std::string	body;
};

#include "Utils.hpp"
#include "Server.hpp"
#include "CgiHandler.hpp"

#endif