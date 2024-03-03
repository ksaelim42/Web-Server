#include "Server.hpp"

Server::Server(void) {
	_initMineTypeDefault();
	_initErrPage();
	_name = "";						// default : null
	_ipAddr = "0.0.0.0";			// default : any addr
	_port = "80";					// default : 80
	_root = "html";					// default : html directory
	_index.push_back("index.html");	// default : index.html
	cliBodySize = BODYBUFSIZE;		// dafault : 1m byte
	INIT_METHOD(allowMethod);		// default : allow all method
	autoIndex = 0;					// defualt : off
	retur.have = 0;					// default : --
	retur.code = 0;
	retur.text = "";
	cgiPass = 0;
}

void	Server::prtServer(void) {
	std::cout << "server_name : " << MAG << _name << RESET << std::endl;
	std::cout << "IP address  : " << MAG << _ipAddr << RESET << std::endl;
	std::cout << "Port        : " << MAG << _port << RESET << std::endl;
	std::cout << "Root        : " << MAG << _root << RESET << std::endl;
	std::cout << "index       : ";
	for (int i = 0; i < _index.size(); i++)
		std::cout << MAG << _index[i] << ", ";
	std::cout << RESET << std::endl;
	std::cout << "CliBodySize : " << MAG << cliBodySize << RESET << std::endl;
	for (int i = 0; i < _location.size(); i++) {
		std::cout << "*** Location[" << i << "] ***" << std::endl;
		std::cout << "path        : " << MAG << _location[i].path << RESET << std::endl;
		std::cout << "Root        : " << MAG << _location[i].root << RESET << std::endl;
		std::cout << "index       : ";
		for (int j = 0; j < _location[i].index.size(); j++)
			std::cout << MAG << _location[i].index[j] << ", ";
		std::cout << RESET << std::endl;
		std::cout << "allowMethod : " << MAG << _location[i].allowMethod << RESET << std::endl;
		std::cout << "autoIndex   : " << MAG << _location[i].autoIndex << RESET << std::endl;
		std::cout << "cliBodySize : " << MAG << _location[i].cliBodySize << RESET << std::endl;
		std::cout << "return      : " << MAG << _location[i].retur.have << ", " << _location[i].retur.code << ", " << _location[i].retur.text << RESET << std::endl;
		std::cout << "cgiPass     : " << MAG << _location[i].cgiPass << RESET << std::endl;
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

// ************************************************************************** //
// ------------------------------- Initialize ------------------------------- //
// ************************************************************************** //

void	Server::_initErrPage(void) {
	errPage[400] = "html/errorPage/400.html";
	errPage[403] = "html/errorPage/403.html";
	errPage[404] = "html/errorPage/404.html";
	errPage[405] = "html/errorPage/405.html";
	errPage[502] = "html/errorPage/502.html";
	errPage[505] = "html/errorPage/505.html";
}

void	Server::_initMineTypeDefault(void) {
	// Text Type
	_mimeType["default"] = "text/plain";
	_mimeType["html"] = "text/html";
	_mimeType["htm"] = "text/html";
	_mimeType["shtml"] = "text/html";
	_mimeType["css"] = "text/css";
	_mimeType["xml"] = "text/xml";
	_mimeType["mml"] = "text/mathml";
	_mimeType["txt"] = "text/plain";
	_mimeType["jad"] = "text/vnd.sun.j2me.app-descriptor";
	_mimeType["wml"] = "text/vnd.wap.wml";
	_mimeType["htc"] = "text/x-component";
	// Images Type
	_mimeType["gif"] = "image/gif";
	_mimeType["jpeg"] = "image/jpeg";
	_mimeType["jpg"] = "image/jpeg";
	_mimeType["png"] = "image/png";
	_mimeType["tif"] = "image/tiff";
	_mimeType["tiff"] = "image/tiff";
	_mimeType["wbmp"] = "image/vnd.wap.wbmp";
	_mimeType["ico"] = "image/x-icon";
	_mimeType["jng"] = "image/x-jng";
	_mimeType["bmp"] = "image/x-ms-bmp";
	_mimeType["svg"] = "image/svg+xml";
	_mimeType["svgz"] = "image/svg+xml";
	_mimeType["webp"] = "image/webp";
	// application it is a top-level media type (not Text or Multimedia)
	_mimeType["js"] = "application/javascript";
	_mimeType["atom"] = "application/atom+xml";
	_mimeType["rss"] = "application/rss+xml";
	_mimeType["woff"] = "application/font-woff";
	_mimeType["jar"] = "application/java-archive";
	_mimeType["war"] = "application/java-archive";
	_mimeType["ear"] = "application/java-archive";
	_mimeType["json"] = "application/json";
	_mimeType["hqx"] = "application/mac-binhex40";
	_mimeType["doc"] = "application/msword";
	_mimeType["pdf"] = "application/pdf";
	_mimeType["ps"] = "application/postscript";
	_mimeType["eps"] = "application/postscript";
	_mimeType["ai"] = "application/postscript";
	_mimeType["rtf"] = "application/rtf";
	_mimeType["m3u8"] = "application/vnd.apple.mpegurl";
	_mimeType["xls"] = "application/vnd.ms-excel";
	_mimeType["eot"] = "application/vnd.ms-fontobject";
	_mimeType["ppt"] = "application/vnd.ms-powerpoint";
	_mimeType["wmlc"] = "application/vnd.wap.wmlc";
	_mimeType["kml"] = "application/vnd.google-earth.kml+xml";
	_mimeType["kmz"] = "application/vnd.google-earth.kmz";
	_mimeType["7z"] = "application/x-7z-compressed";
	_mimeType["cco"] = "application/x-cocoa";
	_mimeType["jardiff"] = "application/x-java-archive-diff";
	_mimeType["jnlp"] = "application/x-java-jnlp-file";
	_mimeType["run"] = "application/x-makeself";
	_mimeType["pl"] = "application/x-perl";
	_mimeType["pm"] = "application/x-perl";
	_mimeType["prc"] = "application/x-pilot";
	_mimeType["pdb"] = "application/x-pilot";
	_mimeType["rar"] = "application/x-rar-compressed";
	_mimeType["rpm"] = "application/x-redhat-package-manager";
	_mimeType["sea"] = "application/x-sea";
	_mimeType["swf"] = "application/x-shockwave-flash";
	_mimeType["sit"] = "application/x-stuffit";
	_mimeType["tcl"] = "application/x-tcl";
	_mimeType["tk"] = "application/x-tcl";
	_mimeType["der"] = "application/x-x509-ca-cert";
	_mimeType["pem"] = "application/x-x509-ca-cert";
	_mimeType["crt"] = "application/x-x509-ca-cert";
	_mimeType["xpi"] = "application/x-xpinstall";
	_mimeType["xhtml"] = "application/xhtml+xml";
	_mimeType["xspf"] = "application/xspf+xml";
	_mimeType["zip"] = "application/zip";
	_mimeType["bin"] = "application/octet-stream";
	_mimeType["exe"] = "application/octet-stream";
	_mimeType["dll"] = "application/octet-stream";
	_mimeType["deb"] = "application/octet-stream";
	_mimeType["dmg"] = "application/octet-stream";
	_mimeType["iso"] = "application/octet-stream";
	_mimeType["img"] = "application/octet-stream";
	_mimeType["msi"] = "application/octet-stream";
	_mimeType["msp"] = "application/octet-stream";
	_mimeType["msm"] = "application/octet-stream";
	_mimeType["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	_mimeType["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	_mimeType["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	// Audio Type
	_mimeType["mid"] = "audio/midi";
	_mimeType["midi"] = "audio/midi";
	_mimeType["kar"] = "audio/midi";
	_mimeType["mp3"] = "audio/mpeg";
	_mimeType["ogg"] = "audio/ogg";
	_mimeType["m4a"] = "audio/x-m4a";
	_mimeType["ra"] = "audio/x-realaudio";
	// Video Type
	_mimeType["3gpp"] = "video/3gpp";
	_mimeType["3gp"] = "video/3gpp";
	_mimeType["ts"] = "video/mp2t";
	_mimeType["mp4"] = "video/mp4";
	_mimeType["mpeg"] = "video/mpeg";
	_mimeType["mpg"] = "video/mpeg";
	_mimeType["mov"] = "video/quicktime";
	_mimeType["webm"] = "video/webm";
	_mimeType["flv"] = "video/x-flv";
	_mimeType["m4v"] = "video/x-m4v";
	_mimeType["mng"] = "video/x-mng";
	_mimeType["asx"] = "video/x-ms-asf";
	_mimeType["asf"] = "video/x-ms-asf";
	_mimeType["wmv"] = "video/x-ms-wmv";
	_mimeType["avi"] = "video/x-msvideo";
}
