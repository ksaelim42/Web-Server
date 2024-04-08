#include "WebServer.hpp"

int	g_state;

WebServer::WebServer() {}

WebServer::~WebServer() {}

bool	WebServer::initServer(std::vector<Server> & servs) {
	struct addrinfo	sockAddr;
	for (size_t i = 0; i < servs.size(); i++) {
		// Creating socket file descriptor
		if (_setSockAddr(sockAddr, servs[i]) == 0)
			throw WebServerException("Setup socket fail");
		servs[i].sockFd = socket(sockAddr.ai_family, sockAddr.ai_socktype, sockAddr.ai_protocol);
		if (servs[i].sockFd < 0)
			throw WebServerException("Create socket fail");
		if (fcntl(servs[i].sockFd, F_SETFL, O_NONBLOCK) < 0)
			throw WebServerException("Non-Blocking i/o fail");
		// To manipulate option for socket and check address is used
		if (_setOptSock(servs[i].sockFd) == 0)
			throw WebServerException("Setup socket fail");
		// Bind the socket to the specified address and port
		if (bind(servs[i].sockFd, sockAddr.ai_addr, sockAddr.ai_addrlen) < 0)
			throw WebServerException("Bind socket fail");
		// Prepare socket for incoming connection
		if (listen(servs[i].sockFd, CONNECTIONSIZE) < 0)
			throw WebServerException("Listen socket fail");
		Logger::isLog(INFO) && Logger::log(GRN, "Success to create server fd: ", servs[i].sockFd);
		_servs.push_back(servs[i]);
	}
	return true;
}

bool	WebServer::runServer(void) {
	int					fd;
	int					nfds;
	// int					timeOut = KEEPALIVETIME * 1000;
	struct epoll_event	ep_event[MAX_EVENTS];

	if (!_setPollFd())
		return false;
	while (g_state) {
		nfds = epoll_wait(_epoll_fd, ep_event, MAX_EVENTS, -1);
		// std::string curTime = currentTime();
		// Logger::isLog(INFO) && Logger::log(GRN, "[Server] - select blocking: ", curTime); // debug
		if (nfds == 0) {
			Logger::isLog(ERROR) && Logger::log(MAG, "[Server] - Time out");
			_timeOutMonitoring();
			continue;
		} else if (nfds == -1) {
			char* err = strerror(errno);
			Logger::isLog(DEBUG) && Logger::log(RED, "[Server] - Error Epoll: ", err);
			continue;
		}
		for (int i = 0; i < nfds; i++) {
			if (ep_event[i].events & EPOLLIN) {
				fd = ep_event[i].data.fd;
				if (_matchServer(fd)) { // if match any servers => new connection
					if (_acceptConnection(fd) < 0) // can't accept connection
						continue;
				}
				else if (_clients.count(fd)) { // client request
					if (_receiveRequest(_clients[fd]) <= 0)
						continue;
				}
				// else if (_cgi.pipes.count(fd)) { // cgi response

				// }
			}
			else if (ep_event[i].events & EPOLLOUT) { // send data back to client
				fd = ep_event[i].data.fd;
				if (_clients.count(fd)) {
					if (_sendResponse(_clients[fd]) <= 0) {
						continue;
					}
					_fdMod(_epoll_fd, fd, EPOLLIN);
				}
				else if (_cgi.pipes.count(fd)) { // cgi request
					_cgi.sendRequest(fd);
					_fdMod(_epoll_fd, fd, EPOLLIN);
				}
			}
		}
		_timeOutMonitoring();
	}
	return true;
}

bool	WebServer::downServer(void) {
	_disconnectAllClient();
	for (size_t i = 0; i < _servs.size(); i++) {
		close(_servs[i].sockFd);
		Logger::isLog(DEBUG) && Logger::log(GRN, "Down server fd: ", _servs[i].sockFd);
	}
	return true;
}

// ************************************************************************** //
// ----------------------------- Server Fields ------------------------------ //
// ************************************************************************** //

int	WebServer::_acceptConnection(int serverFd) {
	Client	client;
	client.sockFd = accept(serverFd, (struct sockaddr *)&client.addr, &client.addrLen);
	if (client.sockFd < 0) {
		Logger::isLog(INFO) && Logger::log(RED, "[Server] - Accept fail");
		return -1;
	} else {
		client.serv = _getServer(serverFd);
		client.IPaddr = inet_ntoa(client.addr.sin_addr);
		if (!client.serv)
			return _disconnectClient(client.sockFd), -1;
		if (_fdAdd(_epoll_fd, client.sockFd, EPOLLIN) == -1)
			return _disconnectClient(client.sockFd), -1;
		_clients[client.sockFd] = client;
		Logger::isLog(INFO) && Logger::log(WHT, "[Server] - Aceept client fd: ", client.sockFd, ", addr: ", client.IPaddr);
	}
	return client.sockFd;
}

int	WebServer::_receiveRequest(Client & client) {
	ssize_t	bytes;

	if (client.getReqType() == HEADER) { // header request
		bytes = recv(client.sockFd, client.buffer, BUFFERSIZE - 1, MSG_DONTWAIT);
		Logger::isLog(DEBUG) && Logger::log(BLU, "[Server] - Receive data ", bytes, " Bytes from client fd: ", client.sockFd);
	}
	else if (client.getReqType() == BODY) { // body request
		bytes = recv(client.sockFd, client.buffer, BUFFERSIZE - 1, MSG_DONTWAIT);
		Logger::isLog(WARNING) && Logger::log(BLU, "[Server] - Receive data ", bytes, " Bytes from client fd: ", client.sockFd);
	}
	else if (client.getReqType() == CHUNK) { // Chunk request
		bytes = _unChunking(client);
		Logger::isLog(WARNING) && Logger::log(BLU, "[Server] - Receive data ", bytes, " Bytes from client fd: ", client.sockFd);
	}
	else
		return 0;
	if (bytes == -1) {
		Logger::isLog(DEBUG) && Logger::log(RED, "[Server] - Error receiving data fd: ", client.sockFd);
		return -1;
	}
	else if (bytes == 0)
		return _disconnectClient(client.sockFd), 0;
	client.buffer[bytes] = '\0';
	Logger::isLog(ERROR) && Logger::log(CYN, "------------------------------");
	Logger::isLog(ERROR) && Logger::log(CYN, client.buffer);
	Logger::isLog(ERROR) && Logger::log(CYN, "------------------------------");
	if (client.getReqType() == HEADER) {
		client.parseRequest(client.buffer, bytes);
	}
	else if (client.getReqType() == BODY || client.getReqType() == CHUNK) {
		_fdMod(_epoll_fd, fd, EPOLLOUT);
	}
	if (client.getReqType() == CGI) {
		if (_cgi.createRequest(&client))
		else
	}
	if (client.getReqType() == RESPONSE) {
		_fdMod(_epoll_fd, fd, EPOLLOUT);
	}
	if (client.getStatus() >= 400) {
		std::string	statusText = HttpResponse::getStatusText(client.getStatus());
		Logger::isLog(DEBUG) && Logger::log(RED, statusText);
	}
	return 1;
}

int	WebServer::_sendResponse(Client & client) {
	ssize_t	bytes;

	client.genResponse(_resMsg);
	bytes = send(client.sockFd, _resMsg.c_str(), _resMsg.length(), 0);
	_resMsg.clear();
	if (bytes < 0) {
		Logger::isLog(DEBUG) && Logger::log(RED, "[Server] - Error to response data");
		return -1;
	}
	Logger::isLog(DEBUG) && Logger::log(CYN, "[Server] - Sent data ", bytes, " Bytes to client fd: ", client.sockFd);
	if (client.getStatus() >= 400) {
		_disconnectClient(client.sockFd);
		return 0;
	}
	return 1;
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
	Logger::isLog(DEBUG) && Logger::log(BLU, "[Server] - Chunck size: ",chunkSize);
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
		count = recv(client.sockFd, &client.buffer[i], chunkSize - i, MSG_DONTWAIT);
		if (count <= 0)
			return count;
		i -= count;
	}
	Logger::isLog(DEBUG) && Logger::log(BLU, "[Server] - Read Chunk Success");
	count = recv(client.sockFd, buf, 2, MSG_DONTWAIT);
	if (count < 2 || buf[0] != '\r' || buf[1] != '\n')
		return client.setResponse(400), 1;
	return chunkSize;
}

// ************************************************************************** //
// --------------------------- Server Initialize ---------------------------- //
// ************************************************************************** //

void	WebServer::signal_handler(int signum) {
	if (signum == SIGQUIT || signum == SIGTERM) {
		Logger::isLog(INFO) && Logger::log(MAG, "\rTerminate Server");
		g_state = 0; // OFF
	}
}

bool	WebServer::_setPollFd(void) {
	g_state = 1; // ON
	signal(SIGQUIT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGPIPE, SIG_IGN); // Protect terminate process when write on the shutdown socket
	// _fdMax = 0;
	// FD_ZERO(&_readFds);
	// FD_ZERO(&_writeFds);
	// _timeOut.tv_sec = KEEPALIVETIME;
	// _timeOut.tv_usec = 0;
	// for (size_t i = 0; i < _servs.size(); i++) {
	// 	_fdSet(_servs[i].sockFd, _readFds);
	// 	Logger::isLog(INFO) && Logger::log(WHT, "Run server name: ", _servs[i].name, ":", _servs[i].port);
	// }
	_epoll_fd = epoll_create(10);
	if (_epoll_fd == -1)
		return (perror("epoll create"), false);
	for (size_t i = 0; i < _servs.size(); i++) {
		if (_fdAdd(_epoll_fd, _servs[i].sockFd, EPOLLIN) == -1) {
			Logger::isLog(INFO) && Logger::log(RED, "Fail to create server: ", _servs[i].name, ":", _servs[i].port);
			return (perror("epoll_ctl"), false);
		}
		Logger::isLog(INFO) && Logger::log(WHT, "Run server: ", _servs[i].name, ":", _servs[i].port);
	}
	return true;
}

bool	WebServer::_setSockAddr(struct addrinfo & sockAddr, Server & serv) {
	int	status;
	struct addrinfo	hints;
	struct addrinfo	*sockAddrTemp;

	memset(&hints, 0, sizeof(hints));	// Set all member to empty
	hints.ai_family = AF_INET;			// Allow IPv4 Protocol
	hints.ai_socktype = SOCK_STREAM;	// Stream socket are socket type for support TCP or SCTP
	hints.ai_protocol = IPPROTO_TCP;	// TCP Protocol
	hints.ai_addr = NULL;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;
	status = getaddrinfo(serv.ipAddr.c_str(), serv.port.c_str(), &hints, &sockAddrTemp);
	if (status != 0) {
		Logger::isLog(DEBUG) && Logger::log(RED, gai_strerror(status));
		return false;
	}
	sockAddr = *sockAddrTemp;
	freeaddrinfo(sockAddrTemp);
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

// ************************************************************************** //
// ---------------------------- Select Function ----------------------------- //
// ************************************************************************** //

// FD_ZERO : memset fd_set
// FD_SET : set fd in fd_set
// FD_CLEAR : remove fd in fd_set
// FD_ISSET : check fd are in fd_set

void	WebServer::_fdSet(int fd, fd_set &set) {
	FD_SET(fd, &set);
	if (fd > _fdMax)
		_fdMax = fd;
}

void	WebServer::_fdClear(int fd, fd_set &set) {
	FD_CLR(fd, &set);
	if (fd == _fdMax)
		_fdMax--;
}

// ************************************************************************** //
// ------------------------ Server Manipulate Clients ----------------------- //
// ************************************************************************** //

bool	WebServer::_matchServer(int &fd) {
	for (size_t i = 0; i < _servs.size(); i++) {
		if (fd == _servs[i].sockFd)
			return true;
	}
	return false;
}

Server*	WebServer::_getServer(int &fd) {
	for (size_t i = 0; i < _servs.size(); i++) {
		if (fd == _servs[i].sockFd)
			return &(_servs[i]);
	}
	Logger::isLog(DEBUG) && Logger::log(RED, "[Server] - There aren't server in list");
	return NULL;
}

void	WebServer::_disconnectClient(int client_fd) {
	if (_clients.count(client_fd)) {
		_fdClear(client_fd, _readFds);
		_fdClear(client_fd, _writeFds);
		close(client_fd);
		_clients.erase(client_fd);
		Logger::isLog(DEBUG) && Logger::log(WHT, "[Server] - Disconnected client fd: ", client_fd);
	}
}

void	WebServer::_disconnectAllClient(void) {
	std::map<int, Client>::iterator	it;
	for (it = _clients.begin(); it != _clients.end(); it++) {
		_fdClear(it->second.sockFd, _readFds);
		_fdClear(it->second.sockFd, _writeFds);
		close(it->second.sockFd);
		Logger::isLog(DEBUG) && Logger::log(WHT, "[Server] - Disconnected client fd: ", it->second.sockFd);
	}
	_clients.clear();
}

void	WebServer::_timeOutMonitoring(void) {
	std::vector<int>	fdList;
	std::time_t	currentTime;

	std::time(&currentTime);
	std::map<int, Client>::iterator	it;
	for (it = _clients.begin(); it != _clients.end(); it++) {
		if (it->second.lastTimeConnected < currentTime)
			fdList.push_back(it->second.sockFd);
	}
	for (size_t i = 0; i < fdList.size(); i++)
		_disconnectClient(fdList[i]);
}

// ************************************************************************** //
// --------------------------- Function Debugging --------------------------- //
// ************************************************************************** //

void	WebServer::_prtFristSet(fd_set &set) {
	long firstFdSet[2];
	size_t	size = sizeof(firstFdSet[0]);

	firstFdSet[0] = set.fds_bits[0];
	firstFdSet[1] = set.fds_bits[1];
	std::cout << MAG << "fd : ";
	for (size_t i = 0; i < size * 2; i++)
		std::cout << i << " ";
	std::cout << "\nset: ";
	for (size_t j = 0; j < 2; j++) {
		for (size_t i = 0; i < size; i++) {
			if ((i + j * 8) > 9)
				std::cout << " ";
			if ((firstFdSet[j] >> i) & 1)
				std::cout << GRN << 1 << " "; 
			else
				std::cout << WHT << 0 << " "; 
		}
	}
	std::cout << RESET << std::endl;
}

// ************************************************************************** //
// ----------------------------- Epoll Function ----------------------------- //
// ************************************************************************** //

// EPOLL_CTL_ADD : add new fd and event to poll_fd
// EPOLL_CTL_MOD : change setting event on fd that are in poll_fd
// EPOLL_CTL_DEL : remove fd in poll_fd

int	WebServer::_fdAdd(int & epoll_fd, int fd, uint32_t events) {
	struct epoll_event	ev;

	ev.data.fd = fd;
	ev.events = events;
	return epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
}

int	WebServer::_fdMod(int & epoll_fd, int fd, uint32_t events) {
	struct epoll_event	ev;

	ev.data.fd = fd;
	ev.events = events;
	return epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev);
}

int	WebServer::_fdDel(int & epoll_fd, int fd) {
	return epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
}