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
#include <fcntl.h>		// fcntl
#include <poll.h>		// poll
#include <stdlib.h>		// malloc
#include <vector>		// vector
#include <sys/epoll.h>	// epoll

#define BLK		"\e[0;30m"
#define RED		"\e[0;31m"
#define GRN		"\e[0;32m"
#define YEL		"\e[0;33m"
#define BLU		"\e[0;34m"
#define MAG		"\e[0;35m"
#define CYN		"\e[0;36m"
#define WHT		"\e[0;37m"
#define RESET	"\e[0m"

# define ADDR			"127.0.0.1"
# define PORT			"1600"
# define BUFFER_SIZE	200

void	prtErr(std::string msg);
int		initServer(char* port);
int		acceptConnection(int & serverSock);
bool	receiveRequest(int client_fd, std::string & request);
bool	readFile(std::string name, std::string &str);
bool	sendReponse(int client_fd, std::string & content);
// fd set manipulate
void fdSet(int &fd, fd_set &set);
void fdClear(int &fd, fd_set &set);
void fdSet(std::vector<struct pollfd> & pfds, int & fd, short event);
void fdClear(std::vector<struct pollfd> & pfds, int & i);
int fdAdd(int & epoll_fd, int & fd, uint32_t events);
int fdMod(int & epoll_fd, int fd, uint32_t events);
int fdDel(int & epoll_fd, int fd);

#endif // !SERVERTOOL_HPP
