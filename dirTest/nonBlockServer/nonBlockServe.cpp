#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fstream>
#include <cstring>
#include <sys/select.h>	// for select

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

bool	prtErr(std::string msg) {
	std::cerr << RED << msg << RESET << std::endl;
	return false;
}

bool	initServer(int & sockFd);
int		acceptConnection(int & serverSock);
bool	receiveRequest(int & client_fd, std::string & request);
#define STDIN_FILENO 0

int main() {
	FD_ZERO(&readfds);
	FD_SET(STDIN_FILENO, &readfds);

	int result = select(STDIN_FILENO + 1, &readfds, NULL, NULL, NULL);
	
	if (result == -1) {
		perror("select");
	} else if (result > 0) {
		if (FD_ISSET(STDIN_FILENO, &readfds)) {
			printf("Data is available for reading on stdin.\n");
			// Read data from stdin
		}
		// Check other sets for write and exceptional conditions if needed
	} else {
		printf("Timeout occurred.\n");
	}

	return 0;
}

int	main (void) {
	int		client_fd;
	int		server_fd;
	fd_set	readfds;
	std::string	reqMsg;
	if (initServer(server_fd) == 0)
		return 1;
	while (1) {
		client_fd = acceptConnection(server_fd);
		if (receiveRequest(client_fd, reqMsg))
		close(client_fd);
		std::cout << GRN << "Close connection" << RESET << std::endl;
	}
	return 0;
}

bool	readFile(std::string name, std::string &str)
{
	std::ifstream	inFile;
	char*			buffer;
	int				length;

	inFile.open(name.c_str());		// Convert string to char* by c_str() function
	if (!inFile.is_open())
	{
		std::cerr << name << " :Can not open." << std::endl;
		return (false);
	}
	inFile.seekg(0, inFile.end);	// Set position to end of the stream
	length = inFile.tellg();		// Get current position
	inFile.seekg(0, inFile.beg);	// Set position back to begining of the stream
	buffer = new char [length];		// Allocate Size of Buffer as size of inFile
	inFile.read(buffer, length);	// Read all data in inFile to Buffer
	str = buffer;					// Copy string from Buffer to str
	delete [] buffer;				// Free Buffer
	inFile.close();					// Close inFile
	return (true);
}

void	handle_client(int client_fd) {
	char	buffer[1024];

	// Receive data from client
	ssize_t	bytes_received =  recv(client_fd, buffer, 1024 - 1, 0);
	if (bytes_received < 0) {
		std::cerr << "Error receiving data" << std::endl;
		close(client_fd);
		exit(1);
	} else if (bytes_received == 0) {
		std::cerr << "Client disconnected" << std::endl;
		close(client_fd);
		exit(1);
	}
	std::cout << "Data receive: " << std::endl;
	// std::cout << "-----------------------------------------" << std::endl;
	// std::cout << buffer << std::endl;
	// std::cout << "-----------------------------------------" << std::endl;
	return ;
}

bool	_setSockAddr(struct addrinfo* & sockAddr) {
	int	status;
	struct addrinfo	hints;
	memset(&hints, 0, sizeof(hints));	// Set all member to empty
	hints.ai_family = AF_UNSPEC;		// Allow IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM;	// Stream socket = TCP
	hints.ai_addr = NULL;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;
	status = getaddrinfo(ADDR, PORT, &hints, &sockAddr);
	if (status != 0) {
		return false;
	}
	return true;
}

bool	_setOptSock(int & sockFd) {
	int	optval = 1;
	if (setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
		return false;
	if (setsockopt(sockFd, IPPROTO_TCP, SO_KEEPALIVE, &optval, sizeof(optval)) < 0)
		return false;
	return true;
}

bool initServer(int & sockFd) {
	struct addrinfo	*sockAddr;
	// Creating socket file descriptor
	if (_setSockAddr(sockAddr) == 0)
		prtErr("Setup socket fail");
	sockFd = socket(sockAddr->ai_family, sockAddr->ai_socktype, sockAddr->ai_protocol);
	if (sockFd < 0)
		prtErr("Create socket fail");
	if (_setOptSock(sockFd) == 0)
		prtErr("Setup socket fail");
	if (bind(sockFd, sockAddr->ai_addr, sockAddr->ai_addrlen) < 0)
		prtErr("Bind socket fail");
	if (listen(sockFd, 10) < 0)
		prtErr("Listen socket fail");
	std::cout << "Domain name: " << MAG << "localhost" << RESET;
	std::cout << ", port: " << MAG << PORT << RESET << std::endl;
	freeaddrinfo(sockAddr);
	return true;
}

int	acceptConnection(int & serverSock) {
	int					client_fd;
	struct sockaddr_in	clientAddr;
	socklen_t			clientAddrLen = sizeof(clientAddr);

	std::cout << GRN << "Waiting for client.." << RESET << std::endl;
	client_fd	= accept(serverSock, (struct sockaddr *)&clientAddr, &clientAddrLen);
	if (client_fd < 0)
		prtErr("Accept fail");
	else {
		std::cout << GRN << "connection accepted from: " << RESET;
		std::cout << "client fd: " << client_fd << ", addr: " << inet_ntoa(clientAddr.sin_addr) << std::endl;
	}
	return client_fd;
}

bool	receiveRequest(int & client_fd, std::string & request) {
	char	buffer[4098];

	memset(buffer, 0, 4098);
	size_t	bytes_received =  recv(client_fd, buffer, 4098 - 1, 0);
	if (bytes_received < 0)
		prtErr("Error receiving data");
	else if (bytes_received == 0)
		prtErr("Client disconnected");
	request = buffer;
	std::cout << "Receive Data: " << bytes_received << " bytes" << std::endl;
	std::cout << "-----------------------------------------" << std::endl;
	std::cout << buffer << std::endl;
	std::cout << "-----------------------------------------" << std::endl;
	return true;
}