#ifndef SERVER_HPP
# define SERVER_HPP

#define HEADBUFSIZE 1000
#define BODYBUFSIZE 1000000

#include <sys/socket.h>
#include <netdb.h>	// for getaddrinfo
#include <cstring>		// for memset

#include "Utils.hpp"

struct return_t {
	short int	code;	// Status Code
	std::string	text;	// Option
};

struct Location
{
	std::string					path;	// path
	std::string					root;
	std::vector<std::string>	index;
	return_t					retur;
};

class Server
{
	private:
		std::string							_name;
		std::string							_ipAddr;
		std::string							_port;
		std::string							_root;
		std::vector<std::string>			_index;
		size_t								_cliHeadSize;
		size_t								_cliBodySize;
		std::vector<Location>				_location;
		std::map<std::string, std::string>	_mimeType;
	public:
		int									sockFd;

		Server(void);
		~Server() {}
		void	setName(std::string name);
		void	setIPaddr(std::string ipAddr);
		void	setPort(std::string port);
		void	setRoot(std::string root);
		void	setIndex(std::string index);
		void	setCliHeadSize(std::string size);
		void	setCliBodySize(std::string size);
		void	setLocation(Location location);
		void	setMimeType(std::string key, std::string value);
		void	prtServer(void);

		std::string							getName(void) const;
		std::string							getIPaddr(void) const;
		std::string							getPort(void) const;
		std::string							getRoot(void) const;
		std::vector<std::string>			getIndex(void) const;
		size_t								getCliHeadSize(void) const;
		size_t								getCliBodySize(void) const;
		std::vector<Location>				getLocation(void) const;
		std::string					getMimeType(const std::string & extension) const;
		struct sockaddr						getSockAddr(void) const;
};

#endif