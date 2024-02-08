#include "Socket.hpp"

int	Socket::acceptConnection(int & serverSock) {
	int					client_fd;
	struct sockaddr_in	clientAddr;
	socklen_t			clientAddrLen = sizeof(clientAddr);

	// Accept connection
	client_fd	= accept(serverSock, (struct sockaddr *)&clientAddr, &clientAddrLen);
	if (client_fd < 0)
		throw SocketException("Accept fail");
	else {
		std::cout << GREEN << "connection accepted from " << RESET << std::endl;
		std::cout << "client fd: " << client_fd \
		<< ", client IP address: " << inet_ntoa(clientAddr.sin_addr) << std::endl;
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
	std::cout << "Receive Data" << std::endl;
	// std::cout << "-----------------------------------------" << std::endl;
	// std::cout << buffer << std::endl;
	// std::cout << "-----------------------------------------" << std::endl;
	return true;
}

bool	Socket::initServer(Server & server) {
	int					status;
	int					server_fd;
	// unsigned short int	netPort;
	int					enable;
	struct sockaddr_in	server_address;

	// Creating socket file descriptor
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
		throw SocketException("Create socket fail");
	// To manipulate option for socket and check address is used
	enable = 1;
	status = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
	if (status < 0)
		throw SocketException("Setup socket fail");
	// Set address and port
	server_address.sin_family = AF_INET; // set for IPv4
	server_address.sin_port = htons(PORT); // convert host byte order to network byte order
	server_address.sin_addr.s_addr = INADDR_ANY; // Bind to all available network interfaces
	// Bind the socket to the specified address and port
	status = bind(server_fd, (struct sockaddr*)&server_address, sizeof(server_address));
	if (status < 0)
		throw SocketException("Bind socket fail");
	// Prepare socket for incoming connection
	status = listen(server_fd, 10);
	if (status < 0)
		throw SocketException("Listen socket fail");
	std::cout << GREEN << "Success to create server" << RESET << std::endl;
	std::cout << "Domain name : localhost" << ", port : " << PORT << std::endl;
	server.fd = server_fd;
	return true;
}

// Tempolary Function
std::string	strCutTo(std::string & str, std::string delimiter) {
	std::size_t	found = str.find_first_of(delimiter);
	std::string	word = str.substr(0, found);
	str.erase(0, found + 1);
	return word;
}

request_t	genRequest(std::string str) {
	request_t	request;
	std::string	root = "content/static";

	// request.path = "./content/static/index.html";
	request.method = strCutTo(str, " ");
	request.path = root + strCutTo(str, " ");
	request.version = strCutTo(str, "\r\n");
	return request;
}

void	prtRequest(request_t request) {
	std::cout << "method: " << request.method << std::endl;
	std::cout << "path: " << request.path << std::endl;
	std::cout << "version: " << request.version << std::endl;
}

bool	Socket::runServer(Server & server) {
	int			client_fd;
	std::string	reqMsg;

	while (1) {
		client_fd = acceptConnection(server.fd);
		if (receiveRequest(client_fd, reqMsg)) {
			_request = genRequest(reqMsg);
			prtRequest(_request);

			HttpResponse	response(server, _request);
			_resMsg = response.createResponse();
			sendResponse(client_fd, _resMsg);
			// http_reponse(client_fd, IMAGE_FILE);
		}
		close(client_fd);
		std::cout << "..." << std::endl;
		sleep(4);
	}
	return true;
}

bool	Socket::downServer(Server & server) {
	close(server.fd);
	std::cout << "close server" << std::endl;
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
