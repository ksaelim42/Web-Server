#ifndef STRUCT_HPP
# define STRUCT_HPP

#include <string>
#include <vector>
#include <sys/stat.h>	// stat

#include "Server.hpp"

# define HEADBUFSIZE	1024		// Buffer size of Response Header in Bytes.
# define BUFFERSIZE		65537		// Buffer size of HTTP request in Bytes.
# define LARGEFILESIZE	10485760	// File size more than 10MB is large file size to response in one times.
# define CONNECTIONSIZE	512			// Numbers of connection from client that server can handle
# define KEEPALIVETIME	5			// Time in second to (keep connection to client)
# define HTTP_VERS		"HTTP/1.1"
# define CGI_VERS		"CGI/1.0"
# define PROGRAM_NAME	"MPM/1.0"	// Mos Prach Mark
# define PYTHON_PATH	"/usr/bin/python3"

enum reqType_e {
	HEADER,
	BODY,
	CHUNK,
	FILE_REQ,
	CGI_REQ,
	RESPONSE,
	DISCARD_DATA
};

enum resType_e {
	ERROR_RES,
	REDIRECT_RES,
	DELETE_RES,
	AUTOINDEX_RES,
	FILE_RES,
	CGI_RES
};

enum bodyType_e {
	NONE_ENCODE,
	CHUNKED_ENCODE
};

struct parsedReq {
	reqType_e							type;
	size_t								package;
	uint64_t							bodySize;
	uint64_t							bodySent;
	bodyType_e							bodyType;
	Server								serv;
	std::string							method;
	std::string							uri;		// original path that get from request
	std::string							version;
	std::string							path;		// path that not include pathinfo, query string, fragment
	std::string							pathInfo;	// path info (come after cgi-script name)
	std::string							queryStr;	// query string (come after `?`)
	std::string							fragment;	// fragment (come after `#`)
	std::string							pathSrc;	// path of source files in Server side
	std::string							body;
	struct stat							fileInfo;	// Src file info
	std::map<std::string, std::string>	headers;
};

#endif