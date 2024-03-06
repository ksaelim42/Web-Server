#ifndef SOCKET_HPP
# define SOCKET_HPP

#include "Utils.hpp"
#include "Server.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <new>
#include <exception>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define HTML_FILE "./content/static/index.html"
#define IMAGE_FILE "./content/static/images/Cat03.jpg"
#define IMAGE_HEADER "\
HTTP/1.1 200 OK\r\n\
Server: nginx/1.22.1\r\n\
Date: Fri, 02 Feb 2024 07:39:12 GMT\r\n\
Content-Type: text/plain\r\n\
Content-Length: 72660\r\n\
Last-Modified: Sun, 28 Jan 2024 09:24:28 GMT\r\n\
Connection: keep-alive\r\n\
ETag: \"65b61d4c-11bd4\"\r\n\
Accept-Ranges: bytes\r\n\r\n\
"

class Socket
{
	private:
		struct addrinfo	*_sockAddr;
		std::string	_name;
		// int		_matchLocation(httpReq & req, std::vector<Server> & servs);
		// int		_matchServer(httpReq &, std::vector<Server> &);
		// int		_matchPath(httpReq & req, Server & serv);
		bool	_setSockAddr(Server &);
		bool	_setOptSock(int &);
	public:
		std::map<std::string, std::string>	mimeType;
		Socket(void) {}
		~Socket();

		bool	initServer(std::vector<Server> &);
		bool	runServer(std::vector<Server> &);
		bool	downServer(std::vector<Server> &);
		int		acceptConnection(int & serverSock);
		bool	receiveRequest(int &, std::string &);
		void	sendResponse(int &, std::string &);
		class	SocketException : public std::exception {
			private:
				std::string	_ErrMsg;
			public:
				SocketException(std::string ErrMsg) {
					_ErrMsg = RED + ErrMsg + RESET;
				}
				virtual const char*	what() const throw() {
					return _ErrMsg.c_str();
				}
				virtual ~SocketException() throw() {}
		};
};

#endif
