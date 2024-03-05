#include "ServerTool.hpp"

bool prtErr(std::string msg)
{
	std::cerr << RED << msg << RESET << std::endl;
	return false;
}

bool _setSockAddr(struct addrinfo *&sockAddr)
{
	int status;
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints)); // Set all member to empty
	hints.ai_family = AF_UNSPEC;	  // Allow IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM;  // Stream socket = TCP
	hints.ai_addr = NULL;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;
	status = getaddrinfo(ADDR, PORT, &hints, &sockAddr);
	if (status != 0)
		return false;
	return true;
}

bool _setOptSock(int &sockFd)
{
	int optval = 1;
	if (setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
		return false;
	if (setsockopt(sockFd, IPPROTO_TCP, SO_KEEPALIVE, &optval, sizeof(optval)) < 0)
		return false;
	return true;
}

bool initServer(int &sockFd)
{
	struct addrinfo *sockAddr;
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

int acceptConnection(int &serverSock)
{
	int client_fd;
	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);

	std::cout << GRN << "Waiting for client.." << RESET << std::endl;
	client_fd = accept(serverSock, (struct sockaddr *)&clientAddr, &clientAddrLen);
	if (client_fd < 0)
		prtErr("Accept fail");
	else
	{
		std::cout << GRN << "connection accepted from: " << RESET;
		std::cout << "client fd: " << client_fd << ", addr: " << inet_ntoa(clientAddr.sin_addr) << std::endl;
	}
	return client_fd;
}

bool receiveRequest(int &client_fd, std::string &request)
{
	char buffer[4098];
	memset(buffer, 0, 4098);
	std::cout << GRN << "Waiting for client request.." << RESET << std::endl;
	size_t bytes_received = recv(client_fd, buffer, 4098 - 1, 0);
	if (bytes_received < 0)
		prtErr("Error receiving data");
	else if (bytes_received == 0)
		prtErr("Client disconnected");
	request = buffer;
	std::cout << BLU << "Receive Data: " << bytes_received << " bytes" << std::endl;
	std::cout << "-----------------------------------------" << std::endl;
	std::cout << buffer << std::endl;
	std::cout << "-----------------------------------------" << RESET << std::endl;
	return true;
}

bool readFile(std::string name, std::string &str)
{
	int		fd;
	int		length;
	char	buffer[4098];
	memset(buffer, 0, 4098);
	fd = open(name.c_str(), O_RDONLY);
	if (fd < 0)
		prtErr(name + " : Can not open.");
	length = read(fd, buffer, 4048);
	str = buffer;
	close(fd);
	return (true);
}

bool sendReponse(int client_fd, std::string & content)
{
	if (send(client_fd, content.c_str(), content.length(), 0) < 0)
		prtErr("Error to response data");
	else
		std::cout << GRN << "Sent data success" << RESET << std::endl;
	return true;
}