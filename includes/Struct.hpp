#ifndef STRUCT_HPP
# define STRUCT_HPP

#include <string>
#include <vector>
#include "Server.hpp"

enum reqType_e {
	HEADER,
	BODY,
	CHUNK,
	RESPONSE
};

struct parsedReq {
	bool								redir;
	reqType_e							type;
	uint64_t							bodySize;
	uint64_t							bodySent;
	Server								serv;
	std::string							cliIPaddr;
	std::string							method;
	std::string							uri;		// original path that get from request
	std::string							version;
	std::string							path;		// path that not include pathinfo, query string, fragment
	std::string							pathInfo;	// path info (come after cgi-script name)
	std::string							queryStr;	// query string (come after `?`)
	std::string							fragment;	// fragment (come after `#`)
	std::string							pathSrc;	// path of source files in Server side
	std::string							body;
	std::map<std::string, std::string>	headers;
};

#endif