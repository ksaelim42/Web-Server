#include "Client.hpp"

Client::Client(void) : _pipeIn(-1), _pipeOut(-1)
, sockFd(-1), pid(-1), bufSize(0), serv(NULL) {
	addrLen = sizeof(struct sockaddr_in);
	updateTime();
	_req.type = HEADER;
}

Client::~Client(void) {
	clearPipeFd();
}

bool	Client::parseHeader(char *buffer, size_t & bufSize) {
	Logger::isLog(WARNING) && Logger::log(BLU, "[Request] - Parsing Header");
	_initRequest();
	std::string	header(buffer, bufSize);
	if (!_divideHeadBody(header))
		return false;
	httpReq	reqHeader;
	if (!storeReq(header, reqHeader))
	{
		status = 400;
		return false;
	}
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
	if (!_findBodySize() || !_findFile() || !_findType())
		return false;
	if (status == 301)
		setResType(REDIRECT_RES);
	else if (_req.serv.cgiPass)
		_req.type = CGI_REQ;
	else if (_req.method == "DELETE")
		setResType(DELETE_RES);
	else if (_req.serv.autoIndex == 1 && S_ISDIR(_req.fileInfo.st_mode))
		setResType(AUTOINDEX_RES);
	else
		_req.type = FILE_REQ;
	return true;
}

void	Client::genResponse(void) {
	if (_res.type == FILE_RES) {
		if (!_res.isBody)
			resMsg = _res.staticContent(this->status, _req);
		else
			resMsg = _res.body;
	}
	else if (_res.type == CGI_RES) {
		if (!_res.isBody)
			resMsg = _res.cgiResponse(this->status, _req);
		else
			resMsg = _res.body;
	}
	else if (_res.type == ERROR_RES) {
		resMsg = _res.errorPage(this->status, _req);
		if (this->status == 413 && _req.bodySize) {
			_req.type = DISCARD_DATA;
			return ;
		}
	}
	else if (_res.type == DELETE_RES)
		resMsg = _res.deleteResource(this->status, _req);
	else if (_res.type == REDIRECT_RES)
		resMsg = _res.redirection(this->status, _req);
	else if (_res.type == AUTOINDEX_RES)
		resMsg = _res.autoIndex(this->status, _req);
	if (getPipeOut() < 0)
		_req.type = HEADER;
}

int	Client::openFile(void) {
	int	fd;

	fd = open(_req.pathSrc.c_str(), O_RDONLY | O_NONBLOCK);
	if (fd < 0) {
		if (errno == ENOENT)	// 2 No such file or directory : 404
			return this->status = 404, -1;
		if (errno == EACCES)	// 13 Permission denied : 403
			return this->status = 403, -1;
		// EMFILE, ENFILE : Too many open file, File table overflow
		// Server Error, May reach the limit of file descriptors : 500
		return this->status = 500, -1;
	}
	_res.bodySize = _req.fileInfo.st_size;
	addPipeFd(fd, PIPE_OUT);
	setResType(FILE_RES);
	return fd;
}

bool	Client::readFile(int fd, char* buffer) {
	ssize_t	bytes;

	bytes = read(fd, buffer, LARGEFILESIZE);
	if (bytes == -1) {
		this->status = 403;
		delPipeFd(fd, PIPE_OUT);
		setResType(ERROR_RES);
		return false;
	}
	_res.body.assign(buffer, bytes);
	_res.bodySent += bytes;
	Logger::isLog(WARNING) && Logger::log(MAG, "[Request] - Read Data form File ", _res.bodySent, " Bytes out of ", _res.bodySize, "Bytes");
	if (_res.bodySent >= _res.bodySize) {
		delPipeFd(fd, PIPE_OUT);
		return true;
	}
	return false;
}

void	Client::updateTime(void) {
	std::time(&lastTimeConnected);		// get current time.
	lastTimeConnected += KEEPALIVETIME;
}

bool	Client::isSizeTooLarge(ssize_t bytes) {
	if (_req.bodySent + bytes > _req.serv.cliBodySize)
		return true;
	return false;
}

// ************************************************************************** //
// ---------------------------- Setter & Getter ----------------------------- //
// ************************************************************************** //

int	Client::getPipeIn(void) const {
	return this->_pipeIn;
}

int	Client::getPipeOut(void) const {
	return this->_pipeOut;
}

void	Client::setReqType(reqType_e type) {
	_req.type = type;
}

void	Client::setResType(resType_e type) {
	_req.type = RESPONSE;
	_res.type = type;
}

reqType_e	Client::getReqType(void) const {
	return _req.type;
}

resType_e	Client::getResType(void) const {
	return _res.type;
}

parsedReq &	Client::getRequest(void) {
	return this->_req;
}

HttpResponse &	Client::getResponse(void) {
	return this->_res;
}

// ************************************************************************** //
// ---------------------------- Parsing Request ----------------------------- //
// ************************************************************************** //

void	Client::_initRequest(void) {
	this->status = 200;
	clearPipeFd();
	resMsg.clear();
	_req.package = 0;
	_req.bodySize = 0;
	_req.bodySent = 0;
	_req.bodyType = NONE_ENCODE;
	_req.serv = *serv;
	_req.pathSrc = "";
	_res.clear();
}

bool	Client::_divideHeadBody(std::string & header) {
	size_t		found;
	std::string	body;
	
	found = header.find("\r\n\r\n");
	if (found == std::string::npos)
		return (this->status = 400, false);
	if (header.length() > found + 4)
		body = header.substr(found + 4);	// Kept only body Message
	bufSize = body.length();
	for (size_t i = 0; i < bufSize; i++)
		buffer[i] = body[i];
	buffer[bufSize] = '\0';
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
			return (this->status = 400, false);
		if (!std::isxdigit(path[found + 1]) || !std::isxdigit(path[found + 2]))
			return (this->status = 400, false);
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
		return (this->status = 405, false);
	if (!_checkVersion(_req.version))
		return (this->status = 505, false);
	return true;
}

bool	Client::_checkMethod(std::string method) {
	if (method == "GET")
		return IS_METHOD_SET(_req.serv.allowMethod, METHOD_GET);
	else if (method == "POST")
		return IS_METHOD_SET(_req.serv.allowMethod, METHOD_POST);
	else if (method == "DELETE")
		return IS_METHOD_SET(_req.serv.allowMethod, METHOD_DEL);
	// else if (method == "HEAD")
	// 	return IS_METHOD_SET(_req.serv.allowMethod, METHOD_HEAD);
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
		this->status = _req.serv.retur.code;
		return true;
	}
	else if (this->status >= 300 && this->status < 400)
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
	Logger::isLog(DEBUG) && Logger::log(RED, "[Request] - File not found");
	return (this->status = 404, false);
}

bool	Client::_findType(void) {
	if (S_ISDIR(_req.fileInfo.st_mode)) { // is directory
		if (_req.path[_req.path.length() - 1] == '/') {
			if (_req.serv.autoIndex)
				return true;
			else
				return (this->status = 403, false);
		}
		else
			return (this->status = 301, true);
	}
	else if (S_ISREG(_req.fileInfo.st_mode))
		return true;
	return (this->status = 404, false);
}

// on POST method request must have `Transfer-Encoding` or `Content-Length` to specify bodySize
bool	Client::_findBodySize(void) {
	if (_req.method != "POST")
		return true;
	if (_req.headers.count("Transfer-Encoding")) {
		if (findHeaderValue(_req.headers, "Transfer-Encoding") != "chunked") {
			Logger::isLog(DEBUG) && Logger::log(RED, "[Request] - Not support Transfer-Encoding Type");
			return this->status = 501, false;
		}
		std::cout << MAG << "Chunk Request" << RESET << std::endl;
		_req.bodyType = CHUNKED_ENCODE;
		return true;
	}
	if (_req.headers.count("Content-Length")) {
		_req.bodySize = strToNum(findHeaderValue(_req.headers, "Content-Length"));
		if (_req.bodySize > _req.serv.cliBodySize) {
			Logger::isLog(DEBUG) && Logger::log(RED, "[Request] - Entity Too Large");
			return this->status = 413, false;
		}
		_req.bodyType = NONE_ENCODE;
		return true;
	}
	Logger::isLog(DEBUG) && Logger::log(RED, "[Request] - Length Required");
	return this->status = 411, false;
}

// ************************************************************************** //
// --------------------------------- Pipe Fd -------------------------------- //
// ************************************************************************** //

void	Client::addPipeFd(int fd, pipe_e direct) {
	if (direct == PIPE_OUT)
		_pipeOut = fd;
	else
		_pipeIn = fd;
	pipeFds[fd] = this;
}

void	Client::delPipeFd(int fd, pipe_e direct) {
	if (pipeFds.count(fd)) {
		if (direct == PIPE_OUT)
			_pipeOut = -1;
		else
			_pipeIn = -1;
		close(fd);
		pipeFds.erase(fd);
	}
}

void	Client::clearPipeFd(void) {
	if (_pipeIn != -1)
		delPipeFd(_pipeIn, PIPE_IN);
	if (_pipeOut != -1)
		delPipeFd(_pipeOut, PIPE_OUT);
	if (pid != -1) {
		kill(pid, SIGKILL);
		Logger::isLog(DEBUG) && Logger::log(YEL, pid, " was killed");
		pid = -1;
	}
}

// ************************************************************************** //
// -------------------------------- Debugging ------------------------------- //
// ************************************************************************** //

void	Client::prtParsedReq(void) {
	std::cout <<  "--- Parsed Request ---" << std::endl;
	std::cout << "status: " << this->status << std::endl;
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
