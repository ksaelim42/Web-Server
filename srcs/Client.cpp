#include "Client.hpp"

Client::Client(void) {
	addrLen = sizeof(struct sockaddr_in);
	serv = NULL;
}

void	Client::parseRequest(std::string reqMsg) {
	httpReq	reqHeader;

	_res.clear();
	reqHeader = storeReq(reqMsg);
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
	_parsePath(_req.uri);
	if (_urlEncoding(_req.path) == 0)
		return;
	_req.serv = *serv;
	_matchLocation(serv->getLocation());
	if (_req.serv.retur.have) // redirection
		return;
	if (_checkRequest() == 0)
		return;
	if (_findFile() == 0)
		return;
	if (_findType() == 0)
		return;
	if (_req.serv.cgiPass)
		_cgi.request(_status, _req);
	return;
}

void	Client::genResponse(std::string & resMsg) {
	if (_req.serv.retur.have || (_status >= 300 && _status < 400))
		resMsg = _res.redirection(_req.serv.retur.code, _req);
	else if (_status == 200 && _req.serv.cgiPass) {
		std::string	cgiMsg;
		_status = _cgi.response(cgiMsg);
		if (_status == 200)
			resMsg = _res.cgiResponse(_status, _req, cgiMsg);
	}
	else if (_status == 200 &&  _req.serv.autoIndex == 1 && S_ISDIR(_fileInfo.st_mode))
		resMsg = _res.autoIndex(_status, _req);
	else if (_status == 200)
		resMsg = _res.staticContent(_status, _req);
	if ((_status >= 400 && _status < 600))
		resMsg = _res.errorPage(_status, _req);
	return;
}

// ************************************************************************** //
// ---------------------------- Parsing Request ----------------------------- //
// ************************************************************************** //

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

// bool	Client::_isCgi(std::string & path) {
// 	size_t	index = path.find_last_of(".");
// 	if (index != std::string::npos) {
// 		std::string	ext = path.substr(index + 1);
// 		if (ext == "sh")
// 			return true;
// 		else if (ext == "pl")
// 			return true;
// 	}
// 	return false;
// }

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
			_req.serv.setRoot(matchLoc.root);
		if (!matchLoc.index.empty())
			_req.serv.setIndex(matchLoc.index);
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
	else if (method == "DEL")
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
	path = _req.serv.getRoot() + _req.path;
	if (path.back() == '/') {
		std::vector<std::string>	index = _req.serv.getIndex();
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
	std::cout << RED << "Not found file in stat" << RESET << std::endl;
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
