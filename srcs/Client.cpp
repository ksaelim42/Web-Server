#include "Client.hpp"

Client::Client(void) {
	_updateTime();
	addrLen = sizeof(struct sockaddr_in);
	serv = NULL;
}

Client::~Client(void) {
	if (pipeIn > 0 && fcntl(pipeIn, F_GETFL) != -1) {
		_closePipe(pipeIn);
		Logger::isLog(WARNING) && Logger::log(YEL, "fd: ", pipeIn, " was closed");
	}
	if (pipeOut > 0 && fcntl(pipeOut, F_GETFL) != -1) {
		_closePipe(pipeOut);
		Logger::isLog(WARNING) && Logger::log(YEL, "fd: ", pipeOut, " was closed");
	}
	if (pid != -1) {
		kill(pid, SIGKILL);
		Logger::isLog(WARNING) && Logger::log(YEL, pid, " was killed");
	}
}

bool	Client::parseHeader(char *buffer, size_t & bufSize) {
	Logger::isLog(WARNING) && Logger::log(BLU, "[Request] - Parsing Header");
	_updateTime();
	_initReqParse();
	std::string	header(buffer, bufSize);
	if (!_divideHeadBody(header))
		return false;
	httpReq	reqHeader = storeReq(header);
	_req.method = reqHeader.method;
	_req.uri = reqHeader.srcPath;
	_req.version = reqHeader.version;
	_req.headers = reqHeader.headers;
	if (!_checkRequest())
		return false;
	_parsePath(_req.uri);
	if (!_urlEncoding(_req.path))
		return false;
	_matchLocation(serv->location);
	if (_redirect()) {
		setResType(REDIRECT_RES);
		return true;
	}
	if (_req.method == "POST" && !_findBodySize())
		return false;
	if (!_findFile())
		return false;
	if (!_findType())
		return false;
	if (_req.serv.cgiPass)
		setResType(CGI_RES);
	else if (_req.method == "DELETE")
		setResType(DELETE_RES);
	else if (_req.serv.autoIndex == 1 && S_ISDIR(_req.fileInfo.st_mode))
		setResType(AUTOINDEX_RES);
	else
		setResType(FILE_RES);
	return true;
}

	// else if (type == BODY || type == CHUNK) {
	// 	if (_cgi.sendBody(buffer, bufSize, _req, type))
	// 		return;

void	Client::genResponse(std::string & resMsg) {
	_updateTime();
	if (_res.type == FILE_RES)
		resMsg = _res.staticContent(_status, _req);
	else if (_res.type == BODY_RES)
		resMsg = _res.body;
	else if (_res.type == ERROR_RES)
		resMsg = _res.errorPage(_status, _req);
	else if (_res.type == DELETE_RES)
		resMsg = _res.deleteResource(_status, _req);
	else if (_res.type == REDIRECT_RES)
		resMsg = _res.redirection(_status, _req);
	else if (_res.type == AUTOINDEX_RES)
		resMsg = _res.autoIndex(_status, _req);
	
	if (_res.type == FILE_RES || _res.type == BODY_RES) {
		if (_req.bodySent >= _req.bodySize) {
			close(pipeOut);
			pipeOut = -1;
			_req.type = HEADER;
		}
		else
			_res.type = BODY_RES;
	}
	// else if (_status == 200 && _req.serv.cgiPass) {
	// 	std::string	cgiMsg;
	// 	_cgi.receiveResponse(_status, cgiMsg);
	// 	resMsg = _res.cgiResponse(_status, _req, cgiMsg);
	// }
	// else if (_status >= 200 && _status < 300) {
	// 	type = OPEN_FILE;
		// int fd = openFile(status, req);
		// if (status >= 200 && status < 300)
		// 	return _createHeader(status, req) + CRLF + _body;

	// }
	// 	resMsg = _res.staticContent(_status, _req);
	// if ((_status >= 400 && _status < 600))
	// 	resMsg = _res.errorPage(_status, _req);
	return;
}

void	Client::prtParsedReq(void) {
	std::cout <<  "--- Parsed Request ---" << std::endl;
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

// ************************************************************************** //
// ---------------------------- Setter & Getter ----------------------------- //
// ************************************************************************** //

short int	Client::getStatus(void) const {
	return _status;
}

reqType_e	Client::getReqType(void) const {
	return _req.type;
}

resType_e	Client::getResType(void) const {
	return _res.type;
}

void	Client::setStatus(short int status) {
	this->_status = status;
}

void	Client::setReqType(reqType_e type) {
	_req.type = type;
}

void	Client::setResType(resType_e type) {
	_req.type = RESPONSE;
	_res.type = type;
}

// short int	Client::getReqType(void) const {return _status;}

// ************************************************************************** //
// ---------------------------- Parsing Request ----------------------------- //
// ************************************************************************** //

void	Client::_initReqParse(void) {
	_status = 200;
	pipeIn = -1;
	pipeOut = -1;
	pid = -1;
	bufSize = 0;
	_req.type = HEADER;
	_req.bodySize = 0;
	_req.bodySent = 0;
	_req.serv = *serv;
	_req.pathSrc = "";
	_res.clear();
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

bool	Client::_matchLocation(std::vector<Location> loc) {
	Location	matchLoc;
	
	matchLoc.path = "";
	// searching the location that most charactor match
	for (size_t i = 0; i < loc.size(); i++) {
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

bool	Client::_redirect(void) {
	if (_req.serv.retur.have) {
		_status = _req.serv.retur.code;
		return true;
	}
	else if (_status >= 300 && _status < 400)
		return true;
	return false;
}

bool	Client::_findFile(void) {
	std::string	path;
	path = _req.serv.root + _req.path;
	if (path[path.length() - 1] == '/') {
		std::vector<std::string>	index = _req.serv.index;
		std::string	filePath;
		for (size_t	i = 0; i < index.size(); i++) {
			filePath = path + index[i];
			if (stat(filePath.c_str(), &_req.fileInfo) == 0) {
				_req.pathSrc = filePath;
				Logger::isLog(DEBUG) && Logger::log(MAG, "[Request] - Find path success, Path: ", _req.pathSrc);
				return true;
			}
		}
	}
	if (stat(path.c_str(), &_req.fileInfo) == 0) {
		_req.pathSrc = path;
		Logger::isLog(DEBUG) && Logger::log(MAG, "[Request] - Find path success, Path: ", _req.pathSrc);
		return true;
	}
	Logger::isLog(DEBUG) && Logger::log(RED, "[Request] - Fine not found");
	return (_status = 404, false);
}

bool	Client::_findType(void) {
	if (S_ISDIR(_req.fileInfo.st_mode)) { // is directory
		if (_req.path[_req.path.length() - 1] == '/') {
			if (_req.serv.autoIndex)
				return true;
			else
				return (_status = 403, false);
		}
		else
			return (_status = 301, true);
	}
	else if (S_ISREG(_req.fileInfo.st_mode))
		return true;
	return (_status = 404, false);
}

// on POST method request must have `Transfer-Encoding` or `Content-Length` to specify bodySize
bool	Client::_findBodySize(void) {
	if (_req.headers.count("Transfer-Encoding")) {
		if (findHeaderValue(_req.headers, "Transfer-Encoding") != "chunked") {
			Logger::isLog(DEBUG) && Logger::log(RED, "[Request] - Not support Transfer-Encoding Type");
			return _status = 501, false;
		}
		std::cout << MAG << "Chunk Request" << RESET << std::endl;
		_req.type = CHUNK;
		return true;
	}
	if (_req.headers.count("Content-Length")) {
		_req.bodySize = strToNum(findHeaderValue(_req.headers, "Content-Length"));
		if (_req.bodySize > _req.serv.cliBodySize) {
			Logger::isLog(DEBUG) && Logger::log(RED, "[Request] - Entity Too Large");
			return _status = 413, false;
		}
		_req.type = BODY;
		return true;
	}
	Logger::isLog(DEBUG) && Logger::log(RED, "[Request] - Length Required");
	return _status = 411, false;
}

// ************************************************************************** //
// -------------------------------- Time Out -------------------------------- //
// ************************************************************************** //

void	Client::_updateTime(void) {
	std::time(&lastTimeConnected);		// get current time.
	lastTimeConnected += KEEPALIVETIME;
}

int	Client::openFile(void) {
	int	fd;

	fd = open(_req.pathSrc.c_str(), O_RDONLY);
	if (fd < 0) {
		if (errno == ENOENT)	// 2 No such file or directory : 404
			return _status = 404, -1;
		if (errno == EACCES)	// 13 Permission denied : 403
			return _status = 403, -1;
		// EMFILE, ENFILE : Too many open file, File table overflow
		// Server Error, May reach the limit of file descriptors : 500
		return _status = 500, -1;
	}
	_req.bodySize = _req.fileInfo.st_size;
	return fd;
}

int	Client::readFile(int fd, char* buffer) {
	ssize_t	bytes;

	if (_req.bodySize < _req.bodySent + LARGEFILESIZE) {
		bytes = read(fd, buffer, _req.bodySize);
		_res.body.assign(buffer, bytes);
	}
	else {
		bytes = read(fd, buffer, LARGEFILESIZE);
		_res.body.assign(buffer, bytes);
	}
	_req.bodySent += bytes;
	setResType(FILE_RES);
	return true;
}