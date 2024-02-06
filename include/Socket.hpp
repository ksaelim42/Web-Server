#ifndef SOCKET_HPP
# define SOCKET_HPP

#include "Webserv.hpp"

#define HEADBUFSIZE 1000
#define BODYBUFSIZE 1000000

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
		size_t	clientHeaderBufferSize;
		size_t	clientMaxBodySize;
		// int		acceptConnection(int);
	public:
		std::map<std::string, std::string>	mimeType;
		Socket(void);
		~Socket() {}

		int	initSocket();
		// int	runSocket(int);
		// class	SocketException : public std::exception {
		// 	private:
		// 		std::string const	_name = "Socket : ";
		// 		std::string	_ErrMsg;
		// 	public:
		// 		SocketException(std::string ErrMsg) {
		// 			_ErrMsg = RED + _name + ErrMsg + RESET;
		// 		}
		// 		const char*	what() const throw() {
		// 			return _ErrMsg.c_str();
		// 		}
		// };
};

#endif