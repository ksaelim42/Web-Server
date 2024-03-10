#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#include <new>
#include <exception>
#include <sys/socket.h>	// Socket Programming
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include "Utils.hpp"
#include "Client.hpp"

#define BUFFERSIZE 1024

class WebServer
{
	private:
		std::vector<Server>		_servs;
		std::map<int, Client>	_clients;
		int						_fdMax;
		fd_set					_readFds;
		fd_set					_writeFds;
		char					_buffer[BUFFERSIZE];
		std::string				_reqMsg;
		std::string				_resMsg;
		struct timeval			_timeOut;

		std::string	_name;
		void	_fdSet(int &, fd_set &);
		void	_fdClear(int &, fd_set &);
		bool	_setSockAddr(struct addrinfo * &, Server &);
		bool	_setOptSock(int &);
		bool	_matchServer(int &);
		int		_acceptConnection(int &);
		bool	_receiveRequest(int &);
		void	_sendResponse(int &, std::string &);
		Server*	_getServer(int &);
	public:
		std::map<std::string, std::string>	mimeType;
		WebServer(std::vector<Server> &);
		~WebServer();

		bool	initServer(void);
		bool	runServer(void);
		bool	downServer(void);
		class	WebServerException : public std::exception {
			private:
				std::string	_ErrMsg;
			public:
				WebServerException(std::string ErrMsg) {
					_ErrMsg = RED + ErrMsg + RESET;
				}
				virtual const char*	what() const throw() {
					return _ErrMsg.c_str();
				}
				virtual ~WebServerException() throw() {}
		};
};

#endif
