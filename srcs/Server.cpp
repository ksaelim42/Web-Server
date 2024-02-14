#include "Server.hpp"

Server::Server(void) {
	// if didn't specify Types context Server will use default Types
	if (1) { //:TODO fix to check config file
		_mimeType = initMineTypeDefault();
	}
	_name = "";
	_ipAddr = "0.0.0.0";
	_port = 80;
	_clientHeaderBufferSize = HEADBUFSIZE;
	_clientMaxBodySize = BODYBUFSIZE;
}

// ************************************************************************** //
// --------------------------------- Setter --------------------------------- //
// ************************************************************************** //

void	Server::setName(const std::string & name) {
	_name = name;
}

void	Server::setIPaddr(const std::string & ipAddr) {

	_ipAddr = ipAddr;
}

void	Server::setPort(const std::string & port) {
	// TODO : between 0 - 65535
	_port = strToNum(port);
}

void	Server::setRoot(const std::string & root) {
	_root = root;
}

void	Server::setIndex(const std::string & index) {
	_index.push_back(index);
}

void	Server::setCliHeadSize(const std::string & size) {
	_clientHeaderBufferSize = strToNum(size);
}

void	Server::setCliBodySize(const std::string & size) {
	_clientMaxBodySize = strToNum(size);
}

void	Server::setLocation(const Location & location) {
	_location.push_back(location);
}

void	Server::setMimeType(const std::string & key, const std::string & value) {
	// if there are have key, They will be over write
	// TODO : may be log to client
	_mimeType[key] = value;
}

// ************************************************************************** //
// --------------------------------- Getter --------------------------------- //
// ************************************************************************** //

std::string	Server::getName(void) const {return _name;}

std::string	Server::getIPaddr(void) const {return _ipAddr;}

uint16_t	Server::getPort(void) const {return _port;}

std::string	Server::getRoot(void) const {return _root;}

std::vector<std::string>	Server::getIndex(void) const {return _index;}

size_t	Server::getCliHeadSize(void) const {return _clientHeaderBufferSize;}

size_t	Server::getCliBodySize(void) const {return _clientMaxBodySize;}

std::vector<Location>	Server::getLocation(void) const {return _location;}

std::string	Server::getMimeType(const std::string & extension) const {
	std::map<std::string, std::string>::const_iterator	it;
	it = _mimeType.find(extension);
	if (it != _mimeType.end())
		return it->second;
	it = _mimeType.find("default");
	return it->second;
}

// struct sockaddr	Server::getSockAddr(void) const {
// 	int	status;
// 	struct sockaddr	sockAddr;
// 	struct addrinfo	hints, *result;

// 	memset(&hints, 0, sizeof(struct addrinfo));
// 	hints.ai_family = AF_UNSPEC;		// Allow IPv4 or IPv6
// 	hints.ai_socktype = SOCK_STREAM;	// Stream socket = TCP
// 	status = getaddrinfo(_ipAddr.c_str(), _name.c_str(), &hints, &result); // TODO : not sure name is right
// 	if (status != 0) {
// 		std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
// 		// return false;
// 	}
// 	sockAddr = result->ai_addr;
// 	return sockAddr;
// }
