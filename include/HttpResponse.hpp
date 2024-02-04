#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "Webserv.hpp"

class HttpResponse
{
	private:
		std::string	_contentType;
		std::string	_contentLength;
		std::string	_connection;
		std::string	_server;
		std::string	_location;
		std::string	_date;

		std::string	getContentType(std::string);
	public:
		HttpResponse(void) {}
		~HttpResponse() {}

		std::string	createResponse();
};

#endif