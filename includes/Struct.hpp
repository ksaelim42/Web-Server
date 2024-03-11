#ifndef STRUCT_HPP
# define STRUCT_HPP

#include <string>
#include <vector>
#include "Server.hpp"

struct parsedReq {
	std::string							cliIPaddr;
	std::string							method;
	std::string							uri;
	std::string							version;
	std::map<std::string, std::string>	headers;
	std::string							contentLength;
	std::string							contentType;
	std::string							path;
	std::string							pathInfo;
	std::string							queryStr;
	std::string							fragment;
	std::string							redirPath;
	std::string							pathSrc;
	std::string							body;
	Server								serv;
};

#endif