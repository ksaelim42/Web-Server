#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#include <new>			// exception on memory
#include <exception>	// exception class
#include <sys/socket.h>	// Socket Programming
#include <arpa/inet.h>	// inet_ntoa TODO : not allow to use
#include <sys/select.h>	// select function

#include "Utils.hpp"
#include "Client.hpp"

// #define BUFFERSIZE 1024		// Buffer size of HTTP request in Bytes.
#define BUFFERSIZE 50000		// Buffer size of HTTP request in Bytes.
#define CONNECTIONSIZE 50	// Numbers of connection from client that server can handle

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
		const struct timeval	_timeOut; // use const for make select not modify value

		std::string	_name;
		void	_fdSet(int &, fd_set &);
		void	_fdClear(int &, fd_set &);
		bool	_setSockAddr(struct addrinfo * &, Server &);
		bool	_setOptSock(int &);
		bool	_matchServer(int &);
		int		_acceptConnection(int &);
		int		_receiveRequest(int &);
		void	_sendResponse(int &, std::string &);
		Server*	_getServer(int &);
		bool	_connectedClient(fd_set &); // TODO : for test
	public:
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

		void	testPersist(Server & server); // TODO : delete it later
};

#endif
