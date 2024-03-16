#include "WebServer.hpp"

int		requestNumber = 1;

WebServer::WebServer(std::vector<Server> & servs) {
	_fdMax = 0;
	FD_ZERO(&_readFds);
	FD_ZERO(&_writeFds);
	_timeOut.tv_sec = 3;
	_timeOut.tv_usec = 0;
	_servs = servs;
}

WebServer::~WebServer() {
}

void	prtLocal(int sockfd) {
	struct sockaddr_in local_addr;
	socklen_t addrlen = sizeof(local_addr);

	// Retrieve the local address and port
	if (getsockname(sockfd, (struct sockaddr *)&local_addr, &addrlen) == -1) {
		perror("Error getting socket name");
		close(sockfd);
	}

	// Display the local address and port
	std::cout << "Local address: " << inet_ntoa(local_addr.sin_addr) << std::endl;
	std::cout << "Local port: " << ntohs(local_addr.sin_port) << std::endl;
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
		// if (fcntl(_servs[i].sockFd, F_SETFL, O_NONBLOCK) < 0)
		// 	throw WebServerException("Non-Blocking i/o fail");
		// To manipulate option for socket and check address is used
		if (_setOptSock(_servs[i].sockFd) == 0)
			throw WebServerException("Setup socket fail");
		// Bind the socket to the specified address and port
		if (bind(_servs[i].sockFd, sockAddr->ai_addr, sockAddr->ai_addrlen) < 0)
			throw WebServerException("Bind socket fail");
		// Prepare socket for incoming connection
		if (listen(_servs[i].sockFd, CONNECTIONSIZE) < 0)
			throw WebServerException("Listen socket fail");
		std::cout << GRN << "Success to create server" << RESET << std::endl;
		if (!_servs[i].name.empty())
			std::cout << "Server URL: "<< MAG << _servs[i].name;
		else
			std::cout << "Server URL: "<< MAG << "localhost";
		std::cout << ":" << _servs[i].port << RESET << std::endl;
		freeaddrinfo(sockAddr);
		_fdSet(_servs[i].sockFd, _readFds);
		prtLocal(_servs[i].sockFd);
	}
	return true;
}

bool	WebServer::runServer(void) {
	fd_set			tmpReadFds;
	fd_set			tmpWriteFds;
	int				status;
	struct timeval	timeOut;

	Server	server = _servs[0]; // TODO improve for multi server later
	// testPersist(server);
	while (true) {
		tmpReadFds = _readFds; // because select will modified fd_set
		tmpWriteFds = _writeFds; // because select will modified fd_set
		timeOut = _timeOut;
		// select will make system motoring three set, block until some fd ready
		std::cout << BLU << "Loop Server..." << RESET << std::endl;
		// if (select(_fdMax + 1, &tmpReadFds, &tmpWriteFds, NULL, &_timeOut) == -1)
		status = select(_fdMax + 1, &tmpReadFds, &tmpWriteFds, NULL, &timeOut);
		if (status == 0) {
			std::cout << "Time out" << std::endl;
			_disconnectAllClient();
			continue;
		} else if (status == -1) {
			perror("select error"); // TODO
			continue;
		}
		for (int fd = 0; fd <= _fdMax; fd++) {
			// std::cout << CYN << "fd: " << fd << RESET << std::endl;
			if (FD_ISSET(fd, &tmpReadFds)) {
				// std::cout << MAG << "fd: " << fd << RESET << std::endl;
				if (_matchServer(fd)) { // if match any servers => new connection
					if (_acceptConnection(fd) < 0) // can't accept connection
						continue;
				}
				else { // handle data from client
					if (_receiveRequest(fd) <= 0)
						continue;
					_fdClear(fd, _readFds);
					_fdSet(fd, _writeFds);
				}
			}
			else if (FD_ISSET(fd, &tmpWriteFds)) { // send data back to client
				_sendResponse(fd);
				_fdClear(fd, _writeFds);
				_fdSet(fd, _readFds);
				// close(fd);
				// std::cout << GRN << "Close connection" << RESET << std::endl;
			}
		}
	}
	return true;
}

bool	WebServer::downServer(void) {
	for (int i = 0; i < _servs.size(); i++)
		close(_servs[i].sockFd);
	std::cout << "Server are closed" << std::endl;
	return true;
}

int	WebServer::_acceptConnection(int & serverSock) {
	Client	client;

	// Accept connection
	std::cout << GRN << "Waiting client connection..." << RESET << std::endl;
	client.sockFd = accept(serverSock, (struct sockaddr *)&client.addr, &client.addrLen);
	if (client.sockFd < 0) {
		std::cout << RED << "Accept fail" << RESET << std::endl;
		return -1;
	} else {
		std::cout << GRN << requestNumber++ << " : connection accepted from: " << RESET;
		std::cout << "client fd: " << client.sockFd << ", addr: " << inet_ntoa(client.addr.sin_addr) << std::endl;
		// fcntl(fd, F_SETFL, SO_KEEPALIVE); // TODO : should check header first
		// fcntl(client.sockFd, F_SETFL, O_NONBLOCK); // TODO : for test
		client.serv = _getServer(serverSock); // TODO : set as select
		_fdSet(client.sockFd, _readFds);
		_clients[client.sockFd] = client;
	}
	return client.sockFd;
}

int	WebServer::_receiveRequest(int & client_fd) {
	// Receive data from client
	if (_clients.count(client_fd)) {
		if (_clients[client_fd].isBody) {

		}
		else {
			memset(_buffer, 0, BUFFERSIZE);
			std::cout << BLU << "fd: " << client_fd << " Read data" << RESET << std::endl;
			ssize_t	bytes =  recv(client_fd, _buffer, BUFFERSIZE, MSG_DONTWAIT);
			// ssize_t	bytes =  recv(client_fd, _buffer, BUFFERSIZE, 0);
			if (bytes == -1) {
				std::cout << RED << "Error receiving data" << RESET << std::endl;
				return -1;
			}
			else if (bytes == 0) {
				_disconnectClient(client_fd);
				return 0;
			}
			std::cout << CYN << "received data from client" << RESET << std::endl;
			// std::cout << _buffer << std::endl;
			_reqMsg = _buffer;
			_clients[client_fd].parseRequest(_reqMsg);
		}
	}
	return 1;
}

void	WebServer::_sendResponse(int & client_fd) {
	// std::cout << BLU << resMsg.length() << " Bytes : Sent data success" << RESET << std::endl; // debug
	// std::cout << YEL << resMsg << RESET << std::endl; // debug
	std::cout << BLU << "fd: " << client_fd << " Write data" << RESET << std::endl;
	if (_clients.count(client_fd)) {
		_clients[client_fd].genResponse(_resMsg);
		// std::cout << _resMsg << std::endl;
		ssize_t	bytes = send(client_fd, _resMsg.c_str(), _resMsg.length(), 0);
		_resMsg.clear();
		if (bytes < 0) {
			std::cerr << "Error to response data" << std::endl;
			return ;
		}
		std::cout << BLU << bytes << " Bytes : Sent data success" << RESET << std::endl;
	}
	return ;
}

bool	WebServer::_setSockAddr(struct addrinfo * & sockAddr, Server & serv) {
	int	status;
	struct addrinfo	hints;

	memset(&hints, 0, sizeof(hints));	// Set all member to empty
	hints.ai_family = AF_INET;			// Allow IPv4 Protocol
	hints.ai_socktype = SOCK_STREAM;	// Stream socket are socket type for support TCP or SCTP
	hints.ai_protocol = IPPROTO_TCP;	// TCP Protocol
	hints.ai_addr = NULL;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;
	status = getaddrinfo(serv.ipAddr.c_str(), serv.port.c_str(), &hints, &sockAddr);
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

bool	WebServer::_connectedClient(fd_set & set) {
	std::map<int, Client>::iterator	it;
	for (it = _clients.begin(); it != _clients.end(); it++) {
		std::cout << YEL << "fd client: " << it->second.sockFd << std::endl;
		if (FD_ISSET(it->second.sockFd, &set))
			return false;
	}
	std::cout << YEL << "non is set" << std::endl;
	return true;
}

void	WebServer::_disconnectClient(int & client_fd) {
	if (_clients.count(client_fd)) {
		_fdClear(client_fd, _readFds);
		_fdClear(client_fd, _writeFds);
		close(client_fd);
		_clients.erase(client_fd);
		std::cerr << RED << "fd: " << client_fd << " Client disconnected" << RESET << std::endl;
	}
}

void	WebServer::_disconnectAllClient(void) {
	std::map<int, Client>::iterator	it;
	for (it = _clients.begin(); it != _clients.end(); it++) {
		_fdClear(it->second.sockFd, _readFds);
		_fdClear(it->second.sockFd, _writeFds);
		close(it->second.sockFd);
		std::cerr << RED << "fd: " << it->second.sockFd << " Client disconnected" << RESET << std::endl;
	}
	_clients.clear();
}

// httpReq	genRequest(std::string str) {
// 	httpReq	req;

// 	req.method = strCutTo(str, " ");
// 	req.srcPath = strCutTo(str, " ");
// 	req.version = strCutTo(str, CRLF);
// 	while (str.compare(0, 2, CRLF) != 0) {
// 		std::string key = strCutTo(str, ": ");
// 		std::string value = strCutTo(str, CRLF);
// 		// std::cout << "key: " << key << std::endl;
// 		// std::cout << "value: " << value << std::endl;
// 		req.headers[key] = value;
// 		// sleep(1);
// 	}
// 	strCutTo(str, CRLF);
// 	req.body = str;
//     str.clear();
// 	return req;
// }

void	WebServer::testPersist(Server & server) {
	int fd = _acceptConnection(server.sockFd);
	int	status;
	std::cout << MAG << "fd: " << fd << RESET << std::endl;
	while (true) {
		std::cout << YEL << "client: " << _clients.size() << ",fd: " << _clients[fd].sockFd << RESET << std::endl;
		status = _receiveRequest(fd);
		if (status == -1) {
			sleep(2);
			continue;
		}
		else if (status == 0)
			exit(0);
		_sendResponse(fd);
		sleep(1);
	}
}
