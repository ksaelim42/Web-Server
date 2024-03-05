#ifndef SERVERTOOL_HPP
# define SERVERTOOL_HPP

#include <iostream>		// std::cout, cin, cerr
#include <sys/socket.h>	// create socket
#include <netdb.h>		// getaddrinfo
#include <arpa/inet.h>	// inet_ntoa function
#include <fcntl.h>		// open file
#include <unistd.h>		// read file
#include <cstring>		// memset
#include <sys/select.h>	// select

#define BLK		"\e[0;30m"
#define RED		"\e[0;31m"
#define GRN		"\e[0;32m"
#define YEL		"\e[0;33m"
#define BLU		"\e[0;34m"
#define MAG		"\e[0;35m"
#define CYN		"\e[0;36m"
#define WHT		"\e[0;37m"
#define RESET	"\e[0m"

# define ADDR	"127.0.0.1"
# define PORT	"1600"

bool	prtErr(std::string msg);
bool	initServer(int & sockFd);
int		acceptConnection(int & serverSock);
bool	receiveRequest(int & client_fd, std::string & request);
bool	readFile(std::string name, std::string &str);
bool	sendReponse(int client_fd, std::string & content);

#endif // !SERVERTOOL_HPP
