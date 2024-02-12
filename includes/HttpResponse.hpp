#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "Webserv.hpp"

class HttpResponse
{
	private:
		// const
		std::vector<std::string>	_allowMethod;
		// input
		Server		_server;
		request_t	_request;

		short int	_statusCode;
		std::string	_header;
		std::string	_body;
		std::string	_contentType;
		std::string	_contentLength;
		std::string	_connection;
		std::string	_location;
		std::string	_date;

		bool		_isCgi(std::string &);
		bool		_checkRequest(void);
		bool		_checkMethod(std::string);
		bool		_checkVersion(std::string);
		// header field
		std::string	_getContentType(std::string &, std::map<std::string, std::string> &);
		std::string	_getContentLength(void);
		std::string	_getDate(void);
		std::string	_getStatusLine(short int &);
		std::string	_getStatusText(short int &);
		// body messages
		bool		_readFile(std::string &, std::string &);
	public:
		HttpResponse(Server &, request_t &);
		~HttpResponse() {}

		std::string	createResponse(void);
		void		prtResponse(void);
};

#endif