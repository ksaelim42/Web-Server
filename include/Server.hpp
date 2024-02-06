#ifndef SERVER_HPP
# define SERVER_HPP

#include "Webserv.hpp"

class Server
{
	private:
		size_t	clientHeaderBufferSize;
		size_t	clientMaxBodySize;
		// int		acceptConnection(int);
	public:
		std::map<std::string, std::string>	mimeType;
		Server(void);
		~Server() {}

		// int	initServer();
		// int	runServer(int);
		// class	ServerException : public std::exception {
		// 	private:
		// 		std::string const	_name = "Server : ";
		// 		std::string	_ErrMsg;
		// 	public:
		// 		ServerException(std::string ErrMsg) {
		// 			_ErrMsg = RED + _name + ErrMsg + RESET;
		// 		}
		// 		const char*	what() const throw() {
		// 			return _ErrMsg.c_str();
		// 		}
		// };
};

#endif