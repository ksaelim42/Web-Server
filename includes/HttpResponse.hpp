#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include <ctime>		// time
#include <dirent.h>		// opendir
#include <fcntl.h>		// open file
#include <sys/stat.h>	// stat
#include <unistd.h>		// read, write
#include <sstream>		// ostringstream
#include <iomanip>		// setw

#include "Utils.hpp"
#include "Struct.hpp"

class HttpResponse
{
	private:
		std::map<std::string, std::string>	_headers;
		std::string							_body;

		// Header Field
		std::string	_createHeader(short int &, parsedReq &);
		std::string	_getContentType(parsedReq &);
		std::string	_getContentLength(void);
		std::string	_getDate(void);
		std::string	_getStatusLine(short int &);
		std::string	_getLocation(parsedReq & req);
		// Body Messages
		short int	_readFile(std::string &, std::string &);
		short int	_listFile(parsedReq &, std::string &);
		// Inspect CGI Response
		short int	_parseCgiHeader(std::string &, std::string	&);
		short int	_inspectCgiHeaders(std::string &);
	public:
		std::string	deleteResource(short int &, parsedReq &);
		std::string	redirection(short int &, parsedReq &);
		std::string	autoIndex(short int &, parsedReq &);
		std::string	staticContent(short int &, parsedReq &);
		std::string	cgiResponse(short int &,  parsedReq &, std::string &);
		std::string	errorPage(short int &, parsedReq &);
		void		clear(void);
		void		prtResponse(void);

		static std::string	getStatusText(short int);
};

#endif