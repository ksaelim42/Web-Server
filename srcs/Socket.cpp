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
	ssize_t	bytes_received =  recv(client_fd, buffer, 4098 - 1, 0);
	if (bytes_received < 0) {
		std::cerr << "Error receiving data" << std::endl;
		return false;
	} else if (bytes_received == 0) {
		std::cerr << "Client disconnected" << std::endl;
		return false;
	}
	request = buffer;
	std::cout << "Receive Data: " << bytes_received << " bytes" << std::endl;
	// std::cout << "-----------------------------------------" << std::endl;
	// std::cout << buffer << std::endl;
	// std::cout << "-----------------------------------------" << std::endl;
	// exit(0);
	return true;
}

bool	Socket::initServer(std::vector<Server> & servs) {
	int	enable = 1;

	for (int i = 0; i < servs.size(); i++) {
		// Creating socket file descriptor
		if (_setSockAddr(servs[i]) == 0)
			throw SocketException("Setup socket fail");
		servs[i].sockFd = socket(_sockAddr->ai_family, _sockAddr->ai_socktype, _sockAddr->ai_protocol); // TODO : fix domain later
		if (servs[i].sockFd < 0)
			throw SocketException("Create socket fail");
		// To manipulate option for socket and check address is used
		if (setsockopt(servs[i].sockFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
			throw SocketException("Setup socket fail");
		// Bind the socket to the specified address and port
		if (bind(servs[i].sockFd, _sockAddr->ai_addr, _sockAddr->ai_addrlen) < 0)
			throw SocketException("Bind socket fail");
		// Prepare socket for incoming connection
		if (listen(servs[i].sockFd, 10) < 0) // TODO : edit max client request
			throw SocketException("Listen socket fail");
		std::cout << GREEN << "Success to create server" << RESET << std::endl;
		if (!servs[i].getName().empty())
			std::cout << "Domain name: "<< PURPLE << servs[i].getName() << RESET;
		else
			std::cout << "Domain name: "<< PURPLE << "localhost" << RESET;
		std::cout << ", port: "<< PURPLE << servs[i].getPort() << RESET << std::endl;
		// freeaddrinfo(_sockAddr);
	}
	return true;
}

// Tempolary Function
std::string	strCutTo(std::string & str, std::string delimiter) {
	std::size_t	found = str.find(delimiter);
	std::string	word = str.substr(0, found);
	str.erase(0, found + delimiter.length());
	return word;
}

httpReq	genRequest(std::string str) {
	httpReq	req;

	req.method = strCutTo(str, " ");
	req.srcPath = strCutTo(str, " ");
	req.version = strCutTo(str, CRLF);
	while (str.compare(0, 2, CRLF) != 0) {
		std::string key = strCutTo(str, ": ");
		std::string value = strCutTo(str, CRLF);;
		// std::cout << "key: " << key << std::endl;
		// std::cout << "value: " << value << std::endl;
		req.headers[key] = value;
		// sleep(1);
	}
	req.body = "";
	return req;
}

void	prtRequest(httpReq request) {
	std::cout <<  "--- HTTP Request ---" << std::endl;
	std::cout << "method: " << request.method << std::endl;
	std::cout << "path: " << request.srcPath << std::endl;
	std::cout << "version: " << request.version << std::endl;
	prtMap(request.headers);
	std::cout <<  "--------------------" << std::endl;
}

bool	Socket::runServer(std::vector<Server> & servs) {
	int			client_fd;
	std::string	reqMsg;

	Server	server = servs[0]; // TODO improve for multi server later
	while (1) {
		client_fd = acceptConnection(server.sockFd);
		if (receiveRequest(client_fd, reqMsg)) {
			_request = genRequest(reqMsg);
			// _request = storeReq(reqMsg);
			std::cerr << "Header Line: " << _request.srcPath << std::endl;
			// prtRequest(_request);
			try {
				HttpResponse	response(server, _request);
				// response.prtParsedReq();
				_resMsg = response.createResponse();
				sendResponse(client_fd, _resMsg);
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

	memset(&hints, 0, sizeof(hints));
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

// return -1, if not match any Servers
// return 0 - Server.size()-1, if match Server
// int	Socket::_matchServer(httpReq & req, std::vector<Server> & servs) {
// 	std::string	tmp = req.headers["Host"];
// 	std::string	reqIPaddr = strCutTo(tmp, ":");
// 	if (reqIPaddr == "localhost")
// 		reqIPaddr = "127.0.0.1";
// 	std::string	reqPort = tmp;
// 	// std::cout << "*******************************" << std::endl;
// 	// std::cout << "reqHost: " << reqIPaddr << std::endl;
// 	// std::cout << "reqPort: " << reqPort << std::endl;
// 	for (int i = 0; i < servs.size(); i++) {
// 		if (reqIPaddr == servs[i].getIPaddr()) {
// 			if (reqPort == servs[i].getPort())
// 				return i;
// 		}
// 	}
// 	return -1;
// }

// // return -1, if not match any location
// // return 0 - location.size()-1 , if match location
// int	Server::_matchPath(httpReq & req, Server & serv) {
// 	std::string	path;

// 	std::vector<Location>	loc = serv.getLocation();
// 	for (int i = 0; i < loc.size(); i++) {
// 		req.path.find(loc.path);
// 		if (loc.path.find(req.path) == 0) {
// 			return i;
// 		}
// 	}
// 	return -1;
// }
