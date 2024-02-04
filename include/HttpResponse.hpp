#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "Server.hpp"
#include "Webserv.hpp"

class HttpResponse
{
	private:
		Server		_server;
		request_t	_request;

		std::string	_contentType;
		std::string	_contentLength;
		std::string	_connection;
		// std::string	_server;
		std::string	_location;
		std::string	_date;

		std::string	getContentType(std::string &, std::map<std::string, std::string> &);
	public:
		HttpResponse(Server &, request_t &);
		~HttpResponse() {}

		std::string	createResponse(void);
		void		prtResponse(void);
};

#endif