#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include <ctime>		// time
#include <dirent.h>		// opendir
#include <fcntl.h>		// open file
#include <sys/stat.h>	// stat
#include <unistd.h>		// read, write
#include <sstream>		// ostringstream
#include <iomanip>		// setw
#include <cerrno>		// errno
#include <cstdio>		// remove file

#include "Utils.hpp"
#include "Struct.hpp"

class HttpResponse
{
	private:
		std::map<std::string, std::string>	_headers;

		// Header Field
		std::string	_createHeader(short int &, parsedReq &);
		std::string	_getContentType(parsedReq &);
		std::string	_getContentLength(parsedReq &);
		std::string	_getDate(void);
		std::string	_getStatusLine(short int &);
		std::string	_getLocation(parsedReq & req);
		// Body Messages
		// short int	_readFile(std::string &, std::string &);
		short int	_listFile(parsedReq &, std::string &);
		// Inspect CGI Response
		short int	_parseCgiHeader(std::string &, std::string	&);
		short int	_inspectCgiHeaders(std::string &);
	public:
		resType_e	type;
		bool		isBody;
		std::string	body;
		uint64_t	bodySize;
		uint64_t	bodySent;

		HttpResponse();
		std::string	deleteResource(short int &, parsedReq &);
		std::string	redirection(short int &, parsedReq &);
		std::string	autoIndex(short int &, parsedReq &);
		std::string	staticContent(short int &, parsedReq &);
		std::string	cgiResponse(short int &,  parsedReq &);
		std::string	errorPage(short int &, parsedReq &);
		void		clear(void);
		void		prtResponse(void);

		static std::string	getStatusText(short int);
		std::string	getType(void) const;
};

#endif