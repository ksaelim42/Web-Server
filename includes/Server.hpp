#ifndef SERVER_HPP
# define SERVER_HPP

#define HEADBUFSIZE 1000
#define BODYBUFSIZE 1000000

#include <sys/socket.h>
#include <netdb.h>	// for getaddrinfo
#include <cstring>		// for memset

#include "Utils.hpp"
#include "Location.hpp"

class Server
{
	private:
		std::string							_name;
		std::string							_ipAddr;
		uint16_t							_port;
		std::string							_root;
		std::vector<std::string>			_index;
		size_t								_clientHeaderBufferSize;
		size_t								_clientMaxBodySize;
		std::vector<Location>				_location;
		std::map<std::string, std::string>	_mimeType;
	public:
		int									sockFd;

		Server(void);
		~Server() {}
		void	setName(const std::string & name);
		void	setIPaddr(const std::string & ipAddr);
		void	setPort(const std::string & port);
		void	setRoot(const std::string & root);
		void	setIndex(const std::string & index);
		void	setCliHeadSize(const std::string & size);
		void	setCliBodySize(const std::string & size);
		void	setLocation(const Location & location);
		void	setMimeType(const std::string & key, const std::string & value);

		std::string							getName(void) const;
		std::string							getIPaddr(void) const;
		uint16_t							getPort(void) const;
		std::string							getRoot(void) const;
		std::vector<std::string>			getIndex(void) const;
		size_t								getCliHeadSize(void) const;
		size_t								getCliBodySize(void) const;
		std::vector<Location>				getLocation(void) const;
		std::string					getMimeType(const std::string & extension) const;
		// struct sockaddr						getSockAddr(void) const;
};

#endif