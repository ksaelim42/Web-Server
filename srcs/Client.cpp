#include "Client.hpp"

Client::Client(void) {
	addrLen = sizeof(struct sockaddr_in);
	serv = NULL;
	isBody = 0;
}

short int	Client::getStatus(void) const {return _status;}

// return true when no another request to read
bool	Client::parseRequest(char *buffer, size_t bufSize) {
	if (isBody) {
		if (_cgi.sendBody(buffer, bufSize, _req) == 0)
			return isBody = 0, true;
		return false;
	}
	// test //
	std::string str(buffer, 7);
	std::string	method = strCutTo(str, " ");
	if (_checkMethod(method) == 0)
		return _status = 400, true;
	// test //
	std::string	header;
	std::string	body;
	httpReq		reqHeader;
	std::string reqMsg(buffer, bufSize);

	_res.clear();
	_req.bodySize = 0;
	_req.bodySent = 0;
	_req.redir = 0;
	_req.serv = *serv;
	if (_parseHeader(reqMsg, header, body) == 0)
		return true;
	// std::cout << BLU << "---- header size: " << header.length() << " ---" << RESET << std::endl;
	// std::cout << BLU << header << RESET << std::endl;
	// std::cout << YEL << "---- body size: " << body.length() << " ---" << RESET << std::endl;
	// std::cout << YEL << body << RESET << std::endl;
	// std::cout << YEL << "------------------------------" << RESET << std::endl;
	reqHeader = storeReq(header);
	// status = scanStartLine(reqHeader);
	// if (status != 0)
	// 	return status;
	// Request Field
	_status = 200;
	_req.pathSrc = "";
	_req.cliIPaddr = "";		// Can't find way
	_req.method = reqHeader.method;
	_req.uri = reqHeader.srcPath;
	_req.version = reqHeader.version;
	_req.headers = reqHeader.headers;
	_req.body = body;
	_parsePath(_req.uri);
	if (_urlEncoding(_req.path) == 0)
		return true;
	_matchLocation(serv->location);
	if (_req.serv.retur.have) { // redirection
		_req.redir = 1;
		_status = _req.serv.retur.code;
		return true;
	}
	if (_checkRequest() == 0)
		return true;
	if (_req.method == "POST" && _findBodySize() == 0)
		return true;
	if (_findFile() == 0)
		return true;
	if (_findType() == 0)
		return true;
	if (_req.serv.cgiPass) {
		if (_cgi.sendRequest(_status, _req))
			return isBody = 1, false;
	}
	return true;
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
	return;
}

// ************************************************************************** //
// ---------------------------- Parsing Request ----------------------------- //
// ************************************************************************** //

bool	Client::_parseHeader(std::string & reqMsg, std::string & header, std::string & body) {
	std::size_t	found  = reqMsg.find("\r\n\r\n");
	if (found == std::string::npos)
		return (_status = 400, false);
	header = reqMsg.substr(0, found + 2);	// Cut the second '\r\n' out
	if (reqMsg.length() > found + 4)
		body = reqMsg.substr(found + 4);	// Kept only body Message
	return (_status = 200, true);
}

// Percent encode
bool	Client::_urlEncoding(std::string & path) {
	std::size_t	found;

	found = path.find_first_of("%");	// return index of found character
	while (found != std::string::npos) {
		if (found + 2 >= path.length())	// if not follow with 2 character
			return (_status = 400 ,false);
		if (!std::isxdigit(path[found + 1]) || !std::isxdigit(path[found + 2]))
			return (_status = 400 ,false);
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

// ************************************************************************** //
// -------------------------- Check Header Fields --------------------------- //
// ************************************************************************** //

bool	Client::_checkRequest(void) {
	if (!_checkMethod(_req.method))
		return (_status = 405, false);
	if (!_checkVersion(_req.version))
		return (_status = 505, false);
	return (_status = 200, true);
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
				return (_status = 200, true);
			}
		}
	}
	if (stat(path.c_str(), &_fileInfo) == 0) {
		_req.pathSrc = path;
		std::cout << "Find path success, path: " << MAG << _req.pathSrc << RESET << std::endl;
		return (_status = 200, true);
	}
	std::cout << RED << path << " :Not found file in stat" << RESET << std::endl;
	return (_status = 404, false);
}

bool	Client::_findType(void) {
	if (S_ISDIR(_fileInfo.st_mode)) { // is directory
		if (_req.path.back() == '/') {
			if (_req.serv.autoIndex)
				return (_status = 200, true);
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
		return true;
	}
	if (_req.headers.count("Content-Length")) {
		_req.bodySize = strToNum(findHeaderValue(_req.headers, "Content-Length"));
		if (_req.bodySize > _req.serv.cliBodySize) {
			std::cout << RED << "Request Entity Too Large" << RESET << std::endl;
			return _status = 413, false;
		}
		return true;
	}
	std::cout << RED << "Length Required" << RESET << std::endl;
	return _status = 411, false;
}