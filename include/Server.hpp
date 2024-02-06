#ifndef SERVER_HPP
# define SERVER_HPP

#define HEADBUFSIZE 1000
#define BODYBUFSIZE 1000000

#include "Webserv.hpp"

class Server
{
	private:
		size_t	clientHeaderBufferSize;
		size_t	clientMaxBodySize;
	public:
		int		fd;

		std::map<std::string, std::string>	mimeType;
		Server(void);
		~Server() {}
};

#endif