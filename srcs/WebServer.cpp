#include "WebServer.hpp"

int	requestNumber = 1;

// Client	client;

WebServer::WebServer(std::vector<Server> & servs) {
	_fdMax = 0;
	FD_ZERO(&_readFds);
	FD_ZERO(&_writeFds);
	_servs = servs;
	_timeOut.tv_sec = 3;
	_timeOut.tv_usec = 0;
}

WebServer::~WebServer() {
}

bool	WebServer::runServer(void) {
	fd_set	tmpReadFds;
	fd_set	tmpWriteFds;

	Server	server = _servs[0]; // TODO improve for multi server later
	while (true) {
		tmpReadFds = _readFds; // because select will modified fd_set
		tmpWriteFds = _writeFds; // because select will modified fd_set
		// select will make system motoring three set, block until some fd ready
		if (select(_fdMax + 1, &tmpReadFds, &tmpWriteFds, NULL, &_timeOut) == -1)
			perror("select error"); // TODO
		for (int fd = 0; fd <= _fdMax; fd++) {
			// std::cout << CYN << "fd: " << fd << RESET << std::endl;
			if (FD_ISSET(fd, &tmpReadFds)) {
				// std::cout << MAG << "fd: " << fd << RESET << std::endl;
				if (_matchServer(fd)) { // if match any servers => new connection
					if (_acceptConnection(fd) == 0) // can't accept connection
						continue;
				}
				else { // handle data from client
					if (_receiveRequest(fd) == 0)
						continue;
					fcntl(fd, F_SETFL, SO_KEEPALIVE); // TODO : should check header first
					_reqMsg = _buffer;
					_clients[fd].parseRequest(_reqMsg);
					_fdClear(fd, _readFds);
					_fdSet(fd, _writeFds);
				}
			}
			else if (FD_ISSET(fd, &tmpWriteFds)) { // send data back to client
				_clients[fd].genResponse(_resMsg);
				_sendResponse(fd, _resMsg);
				// sendReponse(fd, _resMsg);
				_fdClear(fd, _writeFds);
				_fdSet(fd, _readFds);
				// close(fd);
				// std::cout << GRN << "Close connection" << RESET << std::endl;
			}
			// Old code
			// _acceptConnection(server.sockFd);
			// _receiveRequest(client.sockFd);
			// _reqMsg(_buffer);
			// client.parseRequest(str);
			// // client.prtParsedReq(); // debug
			// client.genResponse(_resMsg);
			// _sendResponse(client.sockFd, _resMsg);
			// close(client.sockFd);
		}
	}
	return true;
}

int	WebServer::_acceptConnection(int & serverSock) {
	Client	client;

	// Accept connection
	std::cout << GRN << "Waiting client connection..." << RESET << std::endl;
	client.sockFd = accept(serverSock, (struct sockaddr *)&client.addr, &client.addrLen);
	if (client.sockFd < 0)
		return (perror("Accept fail"), false);
	else {
		std::cout << GRN << requestNumber++ << " : connection accepted from: " << RESET;
		std::cout << "client fd: " << client.sockFd << ", addr: " << inet_ntoa(client.addr.sin_addr) << std::endl;
		fcntl(client.sockFd, F_SETFL, O_NONBLOCK); // TODO : for test
		client.serv = _getServer(serverSock); // TODO : set as select
		_fdSet(client.sockFd, _readFds);
		_clients[client.sockFd] = client;
	}
	return true;
}

bool	WebServer::_receiveRequest(int & client_fd) {
	// Receive data from client
	memset(_buffer, 0, BUFFERSIZE);
	// ssize_t	bytes_received =  recv(client_fd, _buffer, BUFFERSIZE, MSG_DONTWAIT);
	ssize_t	bytes_received =  recv(client_fd, _buffer, BUFFERSIZE, 0);
	if (bytes_received < 0) {
		std::cerr << "Error receiving data" << std::endl;
		return false;
	} else if (bytes_received == 0) {
		if (_clients.count(client_fd)) {
			_fdClear(client_fd, _readFds);
			_fdClear(client_fd, _writeFds);
			close(client_fd);
			_clients.erase(client_fd);
			std::cerr << "Client disconnected" << std::endl;
		}
		return false;
	}
	// it->socond.
	/* std::cout << "Receive Data: " << bytes_received << " bytes" << std::endl; */
	/* std::cout << "-----------------------------------------" << std::endl; */
	/* std::cout << buffer << std::endl; */
	/* std::cout << "-----------------------------------------" << std::endl; */
	/* exit(0); */
	return true;
}

bool	WebServer::initServer(void) {
	struct addrinfo	*sockAddr;
	for (int i = 0; i < _servs.size(); i++) {
		// Creating socket file descriptor
		if (_setSockAddr(sockAddr, _servs[i]) == 0)
			throw WebServerException("Setup socket fail");
		_servs[i].sockFd = socket(sockAddr->ai_family, sockAddr->ai_socktype, sockAddr->ai_protocol);
		if (_servs[i].sockFd < 0)
			throw WebServerException("Create socket fail");
		// To manipulate option for socket and check address is used
		if (_setOptSock(_servs[i].sockFd) == 0)
			throw WebServerException("Setup socket fail");
		// Bind the socket to the specified address and port
		if (bind(_servs[i].sockFd, sockAddr->ai_addr, sockAddr->ai_addrlen) < 0)
			throw WebServerException("Bind socket fail");
		// Prepare socket for incoming connection
		if (listen(_servs[i].sockFd, 10) < 0) // TODO : edit max client request
			throw WebServerException("Listen socket fail");
		std::cout << GRN << "Success to create server" << RESET << std::endl;
		if (!_servs[i].getName().empty())
			std::cout << "Domain name: "<< MAG << _servs[i].getName() << RESET;
		else
			std::cout << "Domain name: "<< MAG << "localhost" << RESET;
		std::cout << ", port: "<< MAG << _servs[i].getPort() << RESET << std::endl;
		freeaddrinfo(sockAddr);
		fcntl(_servs[i].sockFd, F_SETFL, O_NONBLOCK); // TODO : for test
		_fdSet(_servs[i].sockFd, _readFds);
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

bool	WebServer::downServer(void) {
	for (int i = 0; i < _servs.size(); i++)
		close(_servs[i].sockFd);
	std::cout << "Server are closed" << std::endl;
	return true;
}

void	WebServer::_sendResponse(int & client_fd, std::string & resMsg) {
	int	status;

	status = send(client_fd, resMsg.c_str(), resMsg.length(), 0);
	if (status < 0) {
		std::cerr << "Error to response data" << std::endl;
		return ;
	}
	std::cout << "Sent data success" << std::endl;
	return ;
}

bool	WebServer::_setSockAddr(struct addrinfo * & sockAddr, Server & serv) {
	int	status;
	struct addrinfo	hints;

	memset(&hints, 0, sizeof(hints));	// Set all member to empty
	hints.ai_family = AF_UNSPEC;		// Allow IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM;	// Stream socket = TCP
	hints.ai_addr = NULL;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;
	status = getaddrinfo(serv.getIPaddr().c_str(), serv.getPort().c_str(), &hints, &sockAddr);
	if (status != 0) {
		std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
		return false;
	}
	return true;
}

bool	WebServer::_setOptSock(int &sockFd) {
	int	optval = 1;
	// Allows binding the same address and port without waiting for the operating system to release the bound address and port
	if (setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
		return false;
	// Enables sending of keep-alive messages on the TCP socket
	// if (setsockopt(sockFd, IPPROTO_TCP, SO_KEEPALIVE, &optval, sizeof(optval)) < 0)
	// 	return false;
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

void	WebServer::_fdSet(int &fd, fd_set &set) {
	FD_SET(fd, &set);
	if (fd > _fdMax)
		_fdMax = fd;
}

void	WebServer::_fdClear(int &fd, fd_set &set) {
	FD_CLR(fd, &set);
	if (fd == _fdMax)
		_fdMax--;
}

bool	WebServer::_matchServer(int &fd) {
	for (int i = 0; i < _servs.size(); i++) {
		if (fd == _servs[i].sockFd)
			return true;
	}
	return false;
}

Server*	WebServer::_getServer(int &fd) {
	for (int i = 0; i < _servs.size(); i++) {
		if (fd == _servs[i].sockFd)
			return &(_servs[i]);
	}
	std::cout << RED << "Can't get server: That possible" << RESET << std::endl;
	return NULL; // TODO:
}
