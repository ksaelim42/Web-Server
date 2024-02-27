#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# define HTTP_VERS		"HTTP/1.1"
# define PORT			8080

#include <ctime>
#include <exception>
#include <dirent.h>		// opendir
#include <sstream>		// ostringstream
#include <iomanip>		// setw

#include "Utils.hpp"
#include "httpReq.hpp"
#include "Server.hpp"
#include "CgiHandler.hpp"

class HttpResponse
{
	private:
		// const
		std::vector<std::string>	_allowMethod;
		// input
		parsedReq		_req;
		CgiHandler		_cgi;

		short int	_status;
		std::string	_header;
		std::string	_body;
		std::string	_contentType;
		std::string	_contentLength;
		std::string	_connection;
		std::string	_location;
		std::string	_date;
		struct stat	_fileInfo;

		// std::string	_encodeURL(std::string uri);
		// bool		_isCgi(std::string &);
		short int	_checkRequest(void);
		bool		_checkMethod(std::string);
		bool		_checkVersion(std::string);
		// --- Header Field --- //
		bool		_createHeader(void);
		std::string	_getContentType(std::string &);
		std::string	_getContentLength(void);
		std::string	_getDate(void);
		std::string	_getStatusLine(short int &);
		std::string	_getStatusText(short int &);
		std::string	_getLocation(std::string & url);
		// body messages
		short int	_readFile(std::string &, std::string &);
		short int	_listFile(std::string &, std::string &);
		bool		_createErrorPage(short int &, std::string &);
		// parsing request
		std::string	_findContent(std::map<std::string, std::string> &, std::string const &);
		bool		_parsePath(std::string);
		bool		_urlEncoding(std::string & path);
		bool		_matchLocation(std::vector<Location>);
		short int	_findFile(void);
		short int	_findType(void);
	public:
		HttpResponse(Server &, httpReq &);
		~HttpResponse() {}

		std::string	createResponse(void);
		void		prtParsedReq(void);
		void		prtResponse(void);

		class	ReqException : public std::exception {
			public:
				const char* what() const throw();
		};
};

#endif