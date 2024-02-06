#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "Server.hpp"
#include "Webserv.hpp"

class HttpResponse
{
	private:
		// input
		Server		_server;
		request_t	_request;

		short int	_statusCode;
		std::string	_header;
		std::string	_body;
		std::string	_contentType;
		std::string	_contentLength;
		std::string	_connection;
		// std::string	_server;
		std::string	_location;
		std::string	_date;

		bool		readFile(std::string &, std::string &);
		std::string	getContentType(std::string &, std::map<std::string, std::string> &);
		std::string	getContentLength(void);
		std::string	getDate(void);
		std::string	getStatusLine(short int &);
		std::string	getStatusText(short int &);
	public:
		HttpResponse(Server &, request_t &);
		~HttpResponse() {}

		std::string	createResponse(void);
		void		prtResponse(void);
};

#endif