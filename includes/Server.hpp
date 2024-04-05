#ifndef SERVER_HPP
# define SERVER_HPP

#define HEADBUFSIZE 1000
#define BODYBUFSIZE 1000000	// defautl

#define	METHOD_GET	0x1
#define	METHOD_HEAD	0x2
#define	METHOD_POST 0x4
#define	METHOD_DEL	0x8

#define INIT_METHOD(BF) BF |= -1
#define SET_METHOD(BF, N) BF |= N // 1.method from config	2.line 7/8/9/10 as a second parameter
#define CLR_METHOD(BF, N) BF &= ~N
#define IS_METHOD_SET(BF, N) BF & N

#include <netdb.h>		// for getaddrinfo
#include <cstring>		// for memset
#include "Utils.hpp"

struct return_t {
	bool		have;	// if there is a return 1 (true)
	short int	code;	// Status Code
	std::string	text;	// Option
};

struct Location
{
	std::string					path;	// path
	std::string					root;
	std::vector<std::string>	index;
	uint16_t					allowMethod;
	bool						autoIndex;
	uint64_t					cliBodySize;
	return_t					retur;
	bool						cgiPass;
};

class Server // don't forget to create vector to store this class as it may have more than one server in the file.
{
	private:
		std::map<std::string, std::string>	_mimeType; //! no need
	public:
		std::string							name; //? server_name
		std::string							ipAddr; //? listen could be ip address or port
		std::string							port; //? listen
		std::string							root; //? root
		int									sockFd; //! Pmos only
		uint64_t							cliBodySize; //?client_max_body_size
		bool								autoIndex; //?autoindex
		bool								cgiPass; //? cgi_pass
		uint16_t							allowMethod; //? limit_except (use SET_METHOD to store this one)
		return_t							retur; //? return
		std::vector<std::string>			index; //? index
		std::vector<Location>				location; //? location
		std::map<short int, std::string>	errPage; //? error_page

		Server(void);
		~Server(void) {}
		void	_initErrPage(void);
		void	_initMineTypeDefault(void);
		void	setMimeType(std::string key, std::string value);
		void	clearLocation(void);
		void	prtServer(void);

		std::string					getMimeType(const std::string & extension) const;
		std::string					getErrPage(short int &) const;
		struct sockaddr				getSockAddr(void) const;
};


void	printConfig(Server obj);
void	clearLocation(Location &locStruct);

#endif