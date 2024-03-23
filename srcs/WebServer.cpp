#include "WebServer.hpp"

int	run = 1;

void	signal_handler(int signum) {
	std::cout << MAG << "\rTerminate Server"<< RESET << std::endl;
	run = 0;
}

WebServer::WebServer(std::vector<Server> & servs) {
	_fdMax = 0;
	FD_ZERO(&_readFds);
	FD_ZERO(&_writeFds);
	_timeOut.tv_sec = 5;
	_timeOut.tv_usec = 0;
	_servs = servs;
	// signal(SIGPIPE, SIG_IGN); // Protect terminate process when write on the shutdown socket
}

WebServer::~WebServer() { }

bool	WebServer::initServer(void) {
	struct addrinfo	*sockAddr;
	for (int i = 0; i < _servs.size(); i++) {
		// Creating socket file descriptor
		if (_setSockAddr(sockAddr, _servs[i]) == 0)
			throw WebServerException("Setup socket fail");
		_servs[i].sockFd = socket(sockAddr->ai_family, sockAddr->ai_socktype, sockAddr->ai_protocol);
		if (_servs[i].sockFd < 0)
			throw WebServerException("Create socket fail");
		if (fcntl(_servs[i].sockFd, F_SETFL, O_NONBLOCK) < 0)
			throw WebServerException("Non-Blocking i/o fail");
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
	}
	return true;
}

bool	WebServer::runServer(void) {
	fd_set			tmpReadFds;
	fd_set			tmpWriteFds;
	int				status;
	struct timeval	timeOut;
	signal(SIGQUIT, signal_handler);

	// Server	server = _servs[0]; // TODO improve for multi server later
	while (run) {
		tmpReadFds = _readFds; // because select will modified fd_set
		tmpWriteFds = _writeFds; // because select will modified fd_set
		timeOut = _timeOut;
		// select will make system motoring three set, block until some fd ready
		// std::cout << BLU << "Loop Server..." << RESET << std::endl;
		status = select(_fdMax + 1, &tmpReadFds, &tmpWriteFds, NULL, &timeOut);
		// status = select(_fdMax + 1, &tmpReadFds, &tmpWriteFds, NULL, NULL);
		if (status == 0) {
			// std::cout << "Time out" << std::endl;
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
				else { 
					if (_clients.count(fd) == 0)
						continue;
					if (_receiveRequest(_clients[fd]) <= 0)
						continue;
				}
			}
			else if (FD_ISSET(fd, &tmpWriteFds)) { // send data back to client
				if (_clients.count(fd) == 0)
					continue;
				if (_sendResponse(_clients[fd]) <= 0) {
					_fdClear(fd, _writeFds);
					continue;
				}
				_fdClear(fd, _writeFds);
				_fdSet(fd, _readFds);
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

// ************************************************************************** //
// ----------------------------- Server Fields ------------------------------ //
// ************************************************************************** //

int	WebServer::_acceptConnection(int & serverFd) {
	Client	client;
	std::cout << GRN << "Waiting client connection..." << RESET << std::endl;
	client.sockFd = accept(serverFd, (struct sockaddr *)&client.addr, &client.addrLen);
	if (client.sockFd < 0) {
		std::cout << RED << "Accept fail" << RESET << std::endl;
		return -1;
	} else {
		std::cout << "client fd: " << client.sockFd << ", addr: " << inet_ntoa(client.addr.sin_addr) << std::endl;
		client.serv = _getServer(serverFd);
		if (!client.serv)
			return _disconnectClient(client.sockFd), -1;
		_fdSet(client.sockFd, _readFds);
		_clients[client.sockFd] = client;
	}
	return client.sockFd;
}

int	WebServer::_receiveRequest(Client & client) {
	ssize_t	bytes;

	if (client.getReqType() == HEADER) { // header request
		std::cout << BLU << "fd: " << client.sockFd << " Read data" << RESET << std::endl;
		bytes = recv(client.sockFd, _buffer, BUFFERSIZE - 1, MSG_DONTWAIT);
	}
	else if (client.getReqType() == BODY) { // body request
		bytes = recv(client.sockFd, _buffer, BUFFERSIZE - 1, MSG_DONTWAIT);
	}
	else if (client.getReqType() == CHUNK) { // Chunk request
		bytes = _unChunking(client);
	}
	else
		return 0;
	if (bytes == -1)
		return std::cout << RED << "Error receiving data" << RESET << std::endl, -1;
	else if (bytes == 0)
		return _disconnectClient(client.sockFd), 0;
	_buffer[bytes] = '\0';
	std::cout << BLU << bytes << " Bytes : received data from client fd: " << client.sockFd << RESET << std::endl;
	if (client.getReqType() != RESPONSE)
		client.parseRequest(_buffer, bytes);
	if (client.getReqType() == RESPONSE) {
		_fdClear(client.sockFd, _readFds);
		_fdSet(client.sockFd, _writeFds);
	}
	// std::cout << CYN << _buffer << RESET << std::endl;
	// std::cout << CYN << "------------------------------" << RESET << std::endl;
	if (client.getStatus() != 200)
		prtLog(client.getStatus());
	return 1;
}

	// std::cout << BLU << resMsg.length() << " Bytes : Sent data success" << RESET << std::endl; // debug
	// std::cout << YEL << resMsg << RESET << std::endl; // debug
	// std::cout << BLU << "fd: " << client_fd << " Write data" << RESET << std::endl;
		// std::cout << MAG << _resMsg << RESET << std::endl;
int	WebServer::_sendResponse(Client & client) {
	ssize_t	bytes;

	client.genResponse(_resMsg);
	bytes = send(client.sockFd, _resMsg.c_str(), _resMsg.length(), 0);
	_resMsg.clear();
	std::cout << CYN << bytes << " Bytes : Sent data success to client fd: " << client.sockFd << RESET << std::endl;
	if (bytes < 0) {
		std::cerr << "Error to response data" << std::endl;
		return -1;
	}
	if (client.getStatus() >= 400) {
		_disconnectClient(client.sockFd);
		return 0;
	}
	return 1;
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
	std::cout << RED << "Can't get server: That impossible" << RESET << std::endl;
	return NULL;
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

void	WebServer::prtLog(short int status) {
	if (status == 400)
		std::cout << RED << "Bad Request" << RESET << std::endl;
	else if (status == 403)
		std::cout << RED << "Forbidden" << RESET << std::endl;
	else if (status == 404)
		std::cout << RED << "Not Found" << RESET << std::endl;
	else if (status == 405)
		std::cout << RED << "Method Not Allowed" << RESET << std::endl;
	else if (status == 411)
		std::cout << RED << "Length Required" << RESET << std::endl;
	else if (status == 413)
		std::cout << RED << "Request Entity Too Large" << RESET << std::endl;
	else if (status == 500)
		std::cout << RED << "Internal Server Error" << RESET << std::endl;
	else if (status == 501)
		std::cout << RED << "Not Implemented" << RESET << std::endl;
	else if (status == 502)
		std::cout << RED << "Bad Gateway" << RESET << std::endl;
	else if (status == 503)
		std::cout << RED << "Service Unavailable" << RESET << std::endl;
	else if (status == 505)
		std::cout << RED << "HTTP Version Not Supported" << RESET << std::endl;
	else
		std::cout << RED << "Undefined: " << status << RESET << std::endl;
}

ssize_t	WebServer::_unChunking(Client & client) {
	char		buf[4];
	size_t		i;
	size_t		chunkSize;
	ssize_t		count;
	std::string	str;

	for (i = 0; i < 4; i++) {
		count = recv(client.sockFd, &buf[0], 1, MSG_DONTWAIT);
		if (count <= 0)
			return count;
		if (isHexChar(buf[0]))
			str.push_back(buf[0]);
		else
			break;
	}
	if (i == 0)
		return client.setResponse(400), 1;
	chunkSize = hexStrToDec(str);
	std::cout << BLU << "chunk size: " << chunkSize << RESET << std::endl;
	if (chunkSize == 0) { // Last chunk
		count = recv(client.sockFd, buf, 4, MSG_DONTWAIT);
		if (count <= 0)
			return client.setResponse(400), 1;
		else
			return client.setResponse(200), 1;
	}
	else if (chunkSize > BUFFERSIZE - 1)
		return client.setResponse(413), 1;
	count = recv(client.sockFd, buf, 2, MSG_DONTWAIT);
	if (count < 2 || buf[0] != '\r' || buf[1] != '\n')
		return client.setResponse(400), 1;
	i = 0;
	while (i < chunkSize) {
		count = recv(client.sockFd, &_buffer[i], chunkSize - i, MSG_DONTWAIT);
		if (count <= 0)
			return count;
		i -= count;
	}
	std::cout << BLU << "read chunk success" << RESET << std::endl;
	count = recv(client.sockFd, buf, 2, MSG_DONTWAIT);
	if (count < 2 || buf[0] != '\r' || buf[1] != '\n')
		return client.setResponse(400), 1;
	return chunkSize;
}