#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#include <new>			// exception on memory
#include <exception>	// exception class
#include <sys/socket.h>	// Socket Programming
#include <arpa/inet.h>	// inet_ntoa TODO : not allow to use
#include <sys/select.h>	// select function
#include <signal.h>		// signal function

#include "Utils.hpp"
#include "Client.hpp"

class WebServer
{
	private:
		int						_fdMax;
		fd_set					_readFds;
		fd_set					_writeFds;
		char					_buffer[BUFFERSIZE];
		std::string				_reqMsg;
		std::string				_resMsg;
		std::vector<Server>		_servs;
		std::map<int, Client>	_clients;
		struct timeval			_timeOut;

		bool	_setPollFd(void);
		int		_acceptConnection(int &);
		int		_receiveRequest(Client &);
		int		_sendResponse(Client &);
		void	_fdSet(int, fd_set &);
		void	_fdClear(int, fd_set &);
		void	_disconnectClient(int);
		void	_disconnectAllClient(void);
		bool	_setSockAddr(struct addrinfo &, Server &);
		bool	_setOptSock(int &);
		bool	_matchServer(int);
		Server*	_getServer(int);
		ssize_t	_unChunking(Client &);
		void	_timeOutMonitoring(void);
		void	_prtFristSet(fd_set &);
	public:
		WebServer();
		~WebServer();

		bool	initServer(std::vector<Server> &);
		bool	runServer(void);
		bool	downServer(void);
		static void	signal_handler(int);
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
