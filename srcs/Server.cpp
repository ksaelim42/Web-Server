#include "Server.hpp"

void	Server::_initErrPage(void) {
	errPage[400] = "html/errorPage/400.html";
	errPage[403] = "html/errorPage/403.html";
	errPage[404] = "html/errorPage/404.html";
	errPage[405] = "html/errorPage/405.html";
	errPage[505] = "html/errorPage/505.html";
}

Server::Server(void) {
	// if didn't specify Types context Server will use default Types
	if (1) { //:TODO fix to check config file
		_mimeType = initMineTypeDefault();
	}
	_name = "";
	_ipAddr = "0.0.0.0";
	_port = "80";
	_root = "html/default";
	_index.push_back("index.html");
	cliBodySize = BODYBUFSIZE;
	_initErrPage();
	allowMethod = 0;
	INIT_METHOD(allowMethod);	// TODO: in case of not limit method
	// SET_METHOD(allowMethod, METHOD_POST);
}

void	Server::prtServer(void) {
	std::cout << "server_name : " << PURPLE << _name << RESET << std::endl;
	std::cout << "IP address  : " << PURPLE << _ipAddr << RESET << std::endl;
	std::cout << "Port        : " << PURPLE << _port << RESET << std::endl;
	std::cout << "Root        : " << PURPLE << _root << RESET << std::endl;
	std::cout << "index       : ";
	for (int i = 0; i < _index.size(); i++)
		std::cout << PURPLE << _index[i] << ", ";
	std::cout << RESET << std::endl;
	std::cout << "CliBodySize : " << PURPLE << cliBodySize << RESET << std::endl;
	for (int i = 0; i < _location.size(); i++) {
		std::cout << "*** Location[" << i << "] ***" << std::endl;
		std::cout << "path        : " << PURPLE << _location[i].path << RESET << std::endl;
		std::cout << "Root        : " << PURPLE << _location[i].root << RESET << std::endl;
		std::cout << "index       : ";
		for (int j = 0; j < _location[i].index.size(); j++)
			std::cout << PURPLE << _location[i].index[j] << ", ";
		std::cout << RESET << std::endl;
	}
	// TODO : MimeType
}

// ************************************************************************** //
// --------------------------------- Setter --------------------------------- //
// ************************************************************************** //

void	Server::setName(std::string name) {
	_name = name;
}

void	Server::setIPaddr(std::string ipAddr) {

	_ipAddr = ipAddr;
}

void	Server::setPort(std::string port) {
	// TODO : between 0 - 65535
	// _port = strToNum(port);
	_port = port;
}

void	Server::setRoot(std::string root) {
	_root = root;
}

void	Server::setIndex(std::vector<std::string> index) {
	_index = index;
}

void	Server::setLocation(Location location) {
	_location.push_back(location);
}

void	Server::setMimeType(std::string key, std::string value) {
	// if there are have key, They will be over write
	// TODO : may be log to client
	_mimeType[key] = value;
}

// ************************************************************************** //
// --------------------------------- Getter --------------------------------- //
// ************************************************************************** //

std::string	Server::getName(void) const {return _name;}

std::string	Server::getIPaddr(void) const {return _ipAddr;}

std::string	Server::getPort(void) const {return _port;}

std::string	Server::getRoot(void) const {return _root;}

std::vector<std::string>	Server::getIndex(void) const {return _index;}

std::vector<Location>	Server::getLocation(void) const {return _location;}

std::string	Server::getMimeType(const std::string & extension) const {
	std::map<std::string, std::string>::const_iterator	it;
	it = _mimeType.find(extension);
	if (it != _mimeType.end())
		return it->second;
	it = _mimeType.find("default");
	return it->second;
}

std::string	Server::getErrPage(short int & errCode) const {
	std::map<short int, std::string>::const_iterator	it;
	it = errPage.find(errCode);
	if (it != errPage.end())
		return it->second;
	return "";
}

struct sockaddr	Server::getSockAddr(void) const {
	int	status;
	struct sockaddr	sockAddr;
	struct addrinfo	hints, *result;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;		// Allow IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM;	// Stream socket = TCP
	status = getaddrinfo(_ipAddr.c_str(), _port.c_str(), &hints, &result); // TODO : not sure name is right
	if (status != 0) {
		std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
		// return false;
	}
	sockAddr = *(result->ai_addr);
	freeaddrinfo(result);
	return sockAddr;
}

void	Server::clearLocation(void) {
	_location.clear();
}
