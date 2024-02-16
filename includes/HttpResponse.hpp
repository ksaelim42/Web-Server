#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# define HTTP_VERS		"HTTP/1.1"
# define PORT			8080

#include <ctime>
#include <exception>

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

		short int	_status;
		std::string	_header;
		std::string	_body;
		std::string	_contentType;
		std::string	_contentLength;
		std::string	_connection;
		std::string	_location;
		std::string	_date;

		std::string	_encodeURL(std::string uri);
		bool		_isCgi(std::string &);
		bool		_checkRequest(void);
		bool		_checkMethod(std::string);
		bool		_checkVersion(std::string);
		// header field
		bool		_createHeader(void);
		std::string	_getContentType(std::string &);
		std::string	_getContentLength(void);
		std::string	_getDate(void);
		std::string	_getStatusLine(short int &);
		std::string	_getStatusText(short int &);
		// body messages
		bool		_readFile(std::string &, std::string &);
		// parsing request
		std::string	_findContent(std::map<std::string, std::string> &, std::string const &);
	public:
		HttpResponse(Server &, httpReq &);
		~HttpResponse() {}

		std::string	createResponse(void);
		void		prtResponse(void);

		class	ReqException : public std::exception {
			public:
				const char* what() const throw();
		};
};

#endif