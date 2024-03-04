#include "Socket.hpp"

int	requestNumber = 1;

Socket::~Socket() {
	// freeaddrinfo(_sockAddr);
}

int	Socket::acceptConnection(int & serverSock) {
	int					client_fd;
	struct sockaddr_in	clientAddr;
	socklen_t			clientAddrLen = sizeof(clientAddr);

	// Accept connection
	std::cout << GREEN << "Waiting for client request..." << RESET << std::endl;
	client_fd	= accept(serverSock, (struct sockaddr *)&clientAddr, &clientAddrLen);
	if (client_fd < 0)
		throw SocketException("Accept fail");
	else {
		std::cout << GREEN << requestNumber++ << " : connection accepted from: " << RESET;
		std::cout << "client fd: " << client_fd << ", addr: " << inet_ntoa(clientAddr.sin_addr) << std::endl;
	}
	return client_fd;
}

bool	Socket::receiveRequest(int & client_fd, std::string & request) {
	char	buffer[4098];

	// Receive data from client
	memset(buffer, 0, 4098);
	ssize_t	bytes_received =  recv(client_fd, buffer, 4098 - 1, 0);
	if (bytes_received < 0) {
		std::cerr << "Error receiving data" << std::endl;
		return false;
	} else if (bytes_received == 0) {
		std::cerr << "Client disconnected" << std::endl;
		return false;
	}
	request = buffer;
	/* std::cout << "Receive Data: " << bytes_received << " bytes" << std::endl; */
	/* std::cout << "-----------------------------------------" << std::endl; */
	/* std::cout << buffer << std::endl; */
	/* std::cout << "-----------------------------------------" << std::endl; */
	/* exit(0); */
	return true;
}

bool	Socket::initServer(std::vector<Server> & servs) {
	for (int i = 0; i < servs.size(); i++) {
		// Creating socket file descriptor
		if (_setSockAddr(servs[i]) == 0)
			throw SocketException("Setup socket fail");
		servs[i].sockFd = socket(_sockAddr->ai_family, _sockAddr->ai_socktype, _sockAddr->ai_protocol); // TODO : fix domain later
		if (servs[i].sockFd < 0)
			throw SocketException("Create socket fail");
		// To manipulate option for socket and check address is used
		if (_setOptSock(servs[i].sockFd) == 0)
			throw SocketException("Setup socket fail");
		// Bind the socket to the specified address and port
		if (bind(servs[i].sockFd, _sockAddr->ai_addr, _sockAddr->ai_addrlen) < 0)
			throw SocketException("Bind socket fail");
		// Prepare socket for incoming connection
		if (listen(servs[i].sockFd, 10) < 0) // TODO : edit max client request
			throw SocketException("Listen socket fail");
		std::cout << GREEN << "Success to create server" << RESET << std::endl;
		if (!servs[i].getName().empty())
			std::cout << "Domain name: "<< MAG << servs[i].getName() << RESET;
		else
			std::cout << "Domain name: "<< MAG << "localhost" << RESET;
		std::cout << ", port: "<< MAG << servs[i].getPort() << RESET << std::endl;
		// freeaddrinfo(_sockAddr);
	}
	return true;
}

httpReq	genRequest(std::string str) {
	httpReq	req;

	req.method = strCutTo(str, " ");
	req.srcPath = strCutTo(str, " ");
	req.version = strCutTo(str, CRLF);
	while (str.compare(0, 2, CRLF) != 0) {
		std::string key = strCutTo(str, ": ");
		std::string value = strCutTo(str, CRLF);
		// std::cout << "key: " << key << std::endl;
		// std::cout << "value: " << value << std::endl;
		req.headers[key] = value;
		// sleep(1);
	}
	strCutTo(str, CRLF);
	req.body = str;
    str.clear();
	return req;
}
void	prtRequest(httpReq & request) {
	std::cout << BBLU <<  "--- HTTP Header ---" << BLU << std::endl;
	std::cout << "method: " << request.method;
	std::cout << ", path: " << request.srcPath;
	std::cout << ", version: " << request.version << std::endl;
	prtMap(request.headers);
	std::cout << BBLU <<  "--- HTTP Body---" << BLU << std::endl;
    std::cout << request.body << std::endl;
	std::cout << BBLU <<  "********************" << RESET << std::endl;
}

bool	Socket::runServer(std::vector<Server> & servs) {
	int			client_fd;
	std::string	reqMsg;

	Server	server = servs[0]; // TODO improve for multi server later
	while (1) {
		client_fd = acceptConnection(server.sockFd);
		if (receiveRequest(client_fd, reqMsg)) {
			// prach request
				httpReq	request = storeReq(reqMsg);
				// short int	status = scanStartLine(repData.startLine);
				// std::cout <<  << status << std::endl;
			// Normal request
				// httpReq request = genRequest(reqMsg);

			// _request = storeReq(reqMsg);
			// std::cerr << "Header Line: " << _request.srcPath << std::endl;
			// prtRequest(request);
			// exit(0);
            HttpResponse	response(server, request);
			try {
				// response.prtParsedReq();
              std::string   resMsg = response.createResponse();
				sendResponse(client_fd, resMsg);
			}
			catch (std::exception &e) {
				std::cerr << e.what() << std::endl;
			}
		}
		close(client_fd);
		std::cout << "..." << std::endl;
	}
	return true;
}

bool	Socket::downServer(std::vector<Server> & servs) {
	for (int i = 0; i < servs.size(); i++)
		close(servs[i].sockFd);
	std::cout << "Server are closed" << std::endl;
	return true;
}

void	Socket::sendResponse(int & client_fd, std::string & resMsg) {
	int	status;

	status = send(client_fd, resMsg.c_str(), resMsg.length(), 0);
	if (status < 0) {
		std::cerr << "Error to response data" << std::endl;
		return ;
	}
	std::cout << "Sent data success" << std::endl;
	return ;
}

bool	Socket::_setSockAddr(Server & serv) {
	int	status;
	struct addrinfo	hints;

	memset(&hints, 0, sizeof(hints));	// Set all member to empty
	hints.ai_family = AF_UNSPEC;		// Allow IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM;	// Stream socket = TCP
	hints.ai_addr = NULL;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;
	status = getaddrinfo(serv.getIPaddr().c_str(), serv.getPort().c_str(), &hints, &_sockAddr);
	if (status != 0) {
		std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
		return false;
	}
	return true;
}

bool	Socket::_setOptSock(int & sockFd) {
	int	optval = 1;
	// Allows binding the same address and port without waiting for the operating system to release the bound address and port
	if (setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
		return false;
	// Enables sending of keep-alive messages on the TCP socket
	if (setsockopt(sockFd, IPPROTO_TCP, SO_KEEPALIVE, &optval, sizeof(optval)) < 0)
		return false;
	// //Adjusts the size of the receive and send buffers for the socket
	// int buffer_size = 8192;
	// if (setsockopt(sockFd, SOL_SOCKET, SO_RCVBUF, &buffer_size, sizeof(buffer_size)) < 0)
	// 	return false;
	// if (setsockopt(sockFd, SOL_SOCKET, SO_SNDBUF, &buffer_size, sizeof(buffer_size)) < 0)
	// 	return false;
	// // Sets the timeout for receive and send operations on the socket
	// struct timeval timeout;
	// timeout.tv_sec = 10;  // seconds
	// timeout.tv_usec = 0;  // microseconds
	// if (setsockopt(sockFd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
	// 	return false;
	// if (setsockopt(sockFd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0)
	// 	return false;
	return true;
}
