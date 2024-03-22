#include "Client.hpp"

Client::Client(void) {
	addrLen = sizeof(struct sockaddr_in);
	serv = NULL;
	_req.type = HEADER;
}

short int	Client::getStatus(void) const {return _status;}

reqType_e	Client::getReqType(void) const {return _req.type;}

void	Client::setResponse(short int status) {
	if (status == 200) {
		size_t	size = 0;
		_cgi.sendBody(NULL, size, _req);
	}
	_status = status;
	_req.type = RESPONSE;
}

// return true when no another request to read
void	Client::parseRequest(char *buffer, size_t bufSize) {
	if (_req.type == HEADER) {
		_initReqParse();
		_parseHeader(buffer, bufSize);
	}
	else if (_req.type == BODY || _req.type == CHUNK) {
		if (_cgi.sendBody(buffer, bufSize, _req))
			return;
		_status = 502;
		_req.type = RESPONSE;
	}
	return;
}

void	Client::genResponse(std::string & resMsg) {
	if (_status == 200 && _req.method == "DELETE")
		resMsg = _res.deleteResource(_status, _req);
	else if (_req.redir || (_status >= 300 && _status < 400))
		resMsg = _res.redirection(_status, _req);
	else if (_status == 200 && _req.serv.cgiPass) {
		std::string	cgiMsg;
		_cgi.receiveResponse(_status, cgiMsg);
		resMsg = _res.cgiResponse(_status, _req, cgiMsg);
	}
	else if (_status == 200 &&  _req.serv.autoIndex == 1 && S_ISDIR(_fileInfo.st_mode))
		resMsg = _res.autoIndex(_status, _req);
	else if (_status >= 200 && _status < 300)
		resMsg = _res.staticContent(_status, _req);
	if ((_status >= 400 && _status < 600))
		resMsg = _res.errorPage(_status, _req);
	_req.type = HEADER;
	return;
}

// ************************************************************************** //
// ---------------------------- Parsing Request ----------------------------- //
// ************************************************************************** //

bool	Client::_parseHeader(char *buffer, size_t & bufSize) {
	std::cout << "parse Header" << std::endl;
	std::string	header(buffer, bufSize);
	if (!_divideHeadBody(header))
		return _req.type = RESPONSE, false;
	httpReq	reqHeader = storeReq(header);
	_req.method = reqHeader.method;
	_req.uri = reqHeader.srcPath;
	_req.version = reqHeader.version;
	_req.headers = reqHeader.headers;
	if (!_checkRequest())
		return _req.type = RESPONSE, false;
	_parsePath(_req.uri);
	if (!_urlEncoding(_req.path))
		return _req.type = RESPONSE, false;
	_matchLocation(serv->location);
	if (_redirect())
		return _req.type = RESPONSE, true;
	if (_req.method == "POST" && !_findBodySize())
		return _req.type = RESPONSE, false;
	if (!_findFile())
		return _req.type = RESPONSE, false;
	if (!_findType())
		return _req.type = RESPONSE, false;
	if (_req.serv.cgiPass) {
		if (_cgi.sendRequest(_status, _req))
			return true;
		else
			return _req.type = RESPONSE, false;
	}
	return _req.type = RESPONSE, true;
}

bool	Client::_divideHeadBody(std::string & header) {
	size_t	found;
	
	found = header.find("\r\n\r\n");
	if (found == std::string::npos)
		return (_status = 400, false);
	if (header.length() > found + 4)
		_req.body = header.substr(found + 4);	// Kept only body Message
	header = header.substr(0, found + 2);	// Cut the second '\r\n' out
	return true;
}

// Percent encode
bool	Client::_urlEncoding(std::string & path) {
	std::size_t	found;

	found = path.find_first_of("%");	// return index of found character
	while (found != std::string::npos) {
		if (found + 2 >= path.length())	// if not follow with 2 character
			return (_status = 400, false);
		if (!std::isxdigit(path[found + 1]) || !std::isxdigit(path[found + 2]))
			return (_status = 400, false);
		std::string	hex = path.substr(found + 1, 2);
		char	c = std::strtol(hex.c_str(), NULL, 16);
		path.replace(found, 3, 1, c);
		found = path.find_first_of("%");
	}
	return true;
}

// Parsing URI to each path
// 1. fragment
// 2. query string
// 3. path info & path translate
bool	Client::_parsePath(std::string url) {
	std::size_t	found;

	_req.path = "";
	_req.queryStr = "";
	_req.fragment = "";
	found = url.find_last_of("#");
	if (found != std::string::npos) {
		_req.fragment = url.substr(found + 1);
		url = url.substr(0, found);
	}
	found = url.find_last_of("?");
	if (found != std::string::npos) {
		_req.queryStr = url.substr(found + 1);
		url = url.substr(0, found);
	}
	std::string	cgiExt[2] = {".sh", ".py"};
	int	i = 0;
	do {
		found = url.find(cgiExt[i]);
		i++;
	} while (found == std::string::npos && i < 2);
	if (found != std::string::npos) {
		_req.pathInfo = url.substr(found + 3);
		url = url.substr(0, found + 3);
	}
	_req.path = url;
	return true;
}

bool	Client::_matchLocation(std::vector<Location> loc) {
	Location	matchLoc;
	
	matchLoc.path = "";
	// searching the location that most charactor match
	for (int i = 0; i < loc.size(); i++) {
		if (_req.path.find(loc[i].path) == 0 && loc[i].path.size() > matchLoc.path.size())
			matchLoc = loc[i];
	}
	// if Match location use Location
	if (matchLoc.path.size()) {
		if (!matchLoc.root.empty())
			_req.serv.root = matchLoc.root;
		if (!matchLoc.index.empty())
			_req.serv.index = matchLoc.index;
		if (matchLoc.retur.have)
			_req.serv.retur = matchLoc.retur;
		_req.serv.allowMethod = matchLoc.allowMethod;
		_req.serv.autoIndex = matchLoc.autoIndex;
		_req.serv.cliBodySize = matchLoc.cliBodySize;
		_req.serv.cgiPass = matchLoc.cgiPass;
	}
	_req.serv.clearLocation();
	return true;
}

size_t	findCRLF(char * str, size_t & size) {
	for (size_t i = 0; i + 1 < size; i++) {
		if (str[i] == '\r' && str[i + 1] == '\n')
			return i;
	}
	return 0;
}

// ************************************************************************** //
// -------------------------- Check Header Fields --------------------------- //
// ************************************************************************** //

bool	Client::_checkRequest(void) {
	if (!_checkMethod(_req.method))
		return (_status = 405, false);
	if (!_checkVersion(_req.version))
		return (_status = 505, false);
	return true;
}

bool	Client::_checkMethod(std::string method) {
	if (method == "GET")
		return IS_METHOD_SET(_req.serv.allowMethod, METHOD_GET);
	else if (method == "HEAD")
		return IS_METHOD_SET(_req.serv.allowMethod, METHOD_HEAD);
	else if (method == "POST")
		return IS_METHOD_SET(_req.serv.allowMethod, METHOD_POST);
	else if (method == "DELETE")
		return IS_METHOD_SET(_req.serv.allowMethod, METHOD_DEL);
	return false;
}

bool	Client::_checkVersion(std::string version) {
	// HTTP/1.1 and HTTP/1.0 are most used, and HTTP/1.1 can support HTTP/1.0
	if (version.compare(HTTP_VERS) == 0)
		return true;
	else if (version.compare("HTTP/1.0") == 0)
		return true;
	return false;
}

// ************************************************************************** //
// ------------------------------- Find Files ------------------------------- //
// ************************************************************************** //

bool	Client::_findFile(void) {
	std::string	path;
	path = _req.serv.root + _req.path;
	if (path.back() == '/') {
		std::vector<std::string>	index = _req.serv.index;
		std::string	filePath;
		for (size_t	i = 0; i < index.size(); i++) {
			filePath = path + index[i];
			if (stat(filePath.c_str(), &_fileInfo) == 0) {
				_req.pathSrc = filePath;
				std::cout << "Find path success, path: " << MAG << _req.pathSrc << RESET << std::endl;
				return true;
			}
		}
	}
	if (stat(path.c_str(), &_fileInfo) == 0) {
		_req.pathSrc = path;
		std::cout << "Find path success, path: " << MAG << _req.pathSrc << RESET << std::endl;
		return true;
	}
	std::cout << RED << path << " :Not found file in stat" << RESET << std::endl;
	return (_status = 404, false);
}

bool	Client::_findType(void) {
	if (S_ISDIR(_fileInfo.st_mode)) { // is directory
		if (_req.path.back() == '/') {
			if (_req.serv.autoIndex)
				return true;
			else
				return (_status = 403, false);
		}
		else
			return (_status = 301, true);
	}
	else if (S_ISREG(_fileInfo.st_mode))
		return true;
	return (_status = 404, false);
}

void	Client::prtParsedReq(void) {
	std::cout <<  "--- Parsed Request ---" << std::endl;
	std::cout << "cliIPaddr: " << MAG << _req.cliIPaddr << std::endl;
	std::cout << "status: " << _status << std::endl;
	std::cout << "method: " << _req.method << std::endl;
	std::cout << "uri: " << _req.uri << std::endl;
	std::cout << "version: " << _req.version << std::endl;
	std::cout << "pathSrc: " << _req.pathSrc << std::endl;
	std::cout << "path: " << _req.path << std::endl;
	std::cout << "pathInfo: " << _req.pathInfo << std::endl;
	std::cout << "queryStr: " << _req.queryStr << std::endl;
	std::cout << "fragment: " << _req.fragment << std::endl;
	std::cout << "body: " << _req.body << std::endl;
	prtMap(_req.headers);
	std::cout <<  "--- Parsed Server ---" << RESET << std::endl;
	_req.serv.prtServer();
}

void	Client::prtRequest(httpReq & request) {
	std::cout << BBLU <<  "--- HTTP Header ---" << BLU << std::endl;
	std::cout << "method: " << request.method;
	std::cout << ", path: " << request.srcPath;
	std::cout << ", version: " << request.version << std::endl;
	prtMap(request.headers);
	std::cout << BBLU <<  "--- HTTP Body---" << BLU << std::endl;
    std::cout << request.body << std::endl;
	std::cout << BBLU <<  "********************" << RESET << std::endl;
}

// on POST method request must have `Transfer-Encoding` or `Content-Length` to specify bodySize
bool	Client::_findBodySize(void) {
	if (_req.headers.count("Transfer-Encoding")) {
		if (findHeaderValue(_req.headers, "Transfer-Encoding") != "chunked") {
			std::cout << RED << "Not support Transfer-Encoding Type" << RESET << std::endl;
			return _status = 501, false;
		}
		std::cout << MAG << "Chunk Request" << RESET << std::endl;
		return _req.type = CHUNK, true;
	}
	if (_req.headers.count("Content-Length")) {
		_req.bodySize = strToNum(findHeaderValue(_req.headers, "Content-Length"));
		if (_req.bodySize > _req.serv.cliBodySize) {
			std::cout << RED << "Request Entity Too Large" << RESET << std::endl;
			return _status = 413, false;
		}
		return _req.type = BODY, true;
	}
	std::cout << RED << "Length Required" << RESET << std::endl;
	return _status = 411, false;
}

int	Client::_unChunk(char * & buffer, size_t & bufSize) {
	// size_t	first;

	// if (_req.bodySize == 0) {
	// 	first = findCRLF(buffer, bufSize);
	// 	if (first == 0)
	// 		return _status = 400, -1;
	// 	// std::cout << RED << "first: " << first << RESET << std::endl;
	// 	std::string	hexStr(buffer, first);
	// 	if (!isHexStr(hexStr)) {
	// 		// std::cout << RED << "Bad heximal: " << hexStr << RESET << std::endl;
	// 		return _status = 400, -1;
	// 	}
	// 	_req.bodySize = hexStrToDec(hexStr); // Chunk Size
	// 	buffer += first + 2;
	// 	bufSize -= (first + 2);
	// 	std::cout << RED << "hex: " << _req.bodySize << RESET << std::endl;
	// 	if (_req.bodySize + 2 <= bufSize) {
	// 		std::cout << RED << "Full Chunk" << RESET << std::endl;
	// 		_req.body.append(buffer, _req.bodySize);
	// 		// std::cout << RED << "buf[1]: " << (int)buffer[_req.bodySize + 1] << RESET << std::endl;
	// 		// std::cout << RED << "buf[2]: " << (int)buffer[_req.bodySize + 2] << RESET << std::endl;
	// 		if(buffer[_req.bodySize] == '\r' && buffer[_req.bodySize + 1] == '\n') {
	// 			std::cout << RED << "bodySize: " << _req.bodySize << std::endl;
	// 			std::cout << RED << "bufSize: " << bufSize << std::endl;
	// 			if (_req.bodySize + 2 == bufSize)
	// 				return 1;
	// 			else {
	// 				buffer += _req.bodySize + 2;
	// 				bufSize -= (_req.bodySize + 2);
	// 				return 2;
	// 			}
	// 		}
	// 		std::cout << RED << "Wrong form" << RESET << std::endl;
	// 	}
	// 	else {
	// 		_req.body.append(buffer, bufSize - first - 2);
	// 		return 0;
	// 	}
	// }
	// else {
	// 	size_t	mustFilled = _req.bodySize - _req.body.length();
	// 	if (mustFilled + 2 <= bufSize) {
	// 		_req.body.append(buffer, mustFilled);
	// 		std::cout << RED << "mustFilled: " << mustFilled << std::endl;
	// 		std::cout << RED << "bufSize: " << bufSize << std::endl;
	// 		if(buffer[mustFilled] == '\r' && buffer[mustFilled + 1] == '\n') {
	// 			if (mustFilled + 2 == bufSize)
	// 				return 1;
	// 			else {
	// 				buffer += mustFilled + 2;
	// 				bufSize -= (mustFilled + 2);
	// 				return 2;
	// 			}
	// 		}
	// 	}
	// 	else {
	// 		_req.body.append(buffer, bufSize);
	// 		return 0;
	// 	}
	// }
	// std::cout << RED << "fail chunk" << RESET << std::endl;
	// // std::cout << RED << _req.body << RESET << std::endl;
	// return _status = 400, -1;
	return 1;
}

void	Client::_initReqParse(void) {
	_res.clear();
	_status = 200;
	_req.bodySize = 0;
	_req.bodySent = 0;
	_req.redir = 0;
	_req.serv = *serv;
	_req.pathSrc = "";
	_req.cliIPaddr = "";
}

bool	Client::_redirect(void) {
	if (_req.serv.retur.have) {
		_req.redir = 1;
		_status = _req.serv.retur.code;
		return true;
	}
	return false;
}