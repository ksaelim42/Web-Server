#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# define HTTP_VERS		"HTTP/1.1"
# define CONNETION		"keep-alive"
# define PORT			8080

#include <ctime>
#include <exception>
#include <dirent.h>		// opendir
#include <sstream>		// ostringstream
#include <iomanip>		// setw
#include <fcntl.h>		// open file

#include "Utils.hpp"
#include "HttpRequest.hpp"
#include "Server.hpp"
#include "CgiHandler.hpp"

class HttpResponse
{
	private:
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		// CgiHandler		_cgi;

		std::string	_cgiHeader;

		// Header Field
		std::string	_createHeader(short int &, parsedReq &);
		std::string	_getContentType(parsedReq &);
		std::string	_getContentLength(void);
		std::string	_getDate(void);
		std::string	_getStatusLine(short int &);
		std::string	_getStatusText(short int &);
		std::string	_getLocation(parsedReq & req);
		// Body Messages
		short int	_readFile(std::string &, std::string &);
		short int	_listFile(parsedReq &, std::string &);
		// Check CGI Response
		// short int	_parseCgiHeader(std::string &);
		// short int	_inspectCgiHeaders(std::string &);
	public:
		HttpResponse() {}
		~HttpResponse() {}

		std::string	redirection(short int &, parsedReq &);
		std::string	autoIndex(short int &, parsedReq &);
		std::string	staticContent(short int &, parsedReq &);
		std::string	errorPage(short int &, parsedReq &);
		void		clear(void);
		void		prtResponse(void);
};

#endif