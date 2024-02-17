#include "HttpResponse.hpp"

// std::string	HttpResponse::_encodeURL(std::string uri) {
// 	for (int i = 0; i < uri.length(); i++) {
// 		if (uri[i] == '%') {
// 			if (!std::isdigit(uri[i]))
// 				throw HttpResponse::ReqException();
// 			else
// 		}
// 	}
// }

bool	HttpResponse::_matchLocation(std::vector<Location> loc) {
	Location	matchLoc;
	
	matchLoc.path = "";
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
		return true;
	}
	else
		return false;
}

HttpResponse::HttpResponse(Server & serv, httpReq & req) {
	// _allowMethod.push_back("GET");
	// _allowMethod.push_back("POST");
	// _allowMethod.push_back("HEAD");
	_req.serv = serv;
	_req.cliIPaddr = "";		// Can't find way
	_req.method = req.method;
	_req.uri = req.srcPath;
	_req.version = req.version;
	_req.headers = req.headers;
	_req.contentLength = _findContent(_req.headers, "Content-Length");
	_req.contentType = _findContent(_req.headers, "Content-Type");
	_splitPath(_req.uri);
	_req.pathInfo = "";
	_req.body = req.body;
	_matchLocation(_req.serv.getLocation());
	_req.serv.clearLocation();
}

std::string	HttpResponse::createResponse(void) {
	_findFile();
	if (_isCgi(_req.path)) {
		CgiHandler	cgi;

		_status = cgi.execCgiScript(_req, _body);
	}
	else {
		_readFile(_req.path, _body);
	}
	_createHeader();
	return _header + CRLF + _body;
}

bool	HttpResponse::_createHeader(void) {
	_contentType = _getContentType(_req.path);
	_contentLength = _getContentLength();
	_date = _getDate();
	_header = _getStatusLine(_status);
	_header += _contentLength;
	_header += _contentType;
	_header += _date;
	return true;
}

bool	HttpResponse::_checkRequest(void) {
	if (!_checkMethod("GET")) { // TODO : get from request
		_status = 405;
		return false;
	}
	if (!_checkVersion("HTTP/1.1")) { // TODO get from request
		_status = 505;
		return false;
	}
	return true;
}

void	HttpResponse::prtResponse(void) {
	std::cout << _contentType;
	std::cout << _contentLength;
	std::cout << _date;
	std::cout << "-----------------------------------------" << std::endl;
	std::cout << _body;
}

// ************************************************************************** //
// -------------------------- Check Header Fields --------------------------- //
// ************************************************************************** //

bool	HttpResponse::_checkMethod(std::string method) {
	for (size_t i = 0; i < _allowMethod.size(); i++) {
		if (method.compare(_allowMethod[i]) == 0)
			return true;
	}
	return false;
}

bool	HttpResponse::_checkVersion(std::string version) {
	if (version.compare(HTTP_VERS) == 0)
		return true;
	return false;
}

// ************************************************************************** //
// ------------------------- Response Header Fields ------------------------- //
// ************************************************************************** //

std::string	HttpResponse::_getStatusLine(short int & statusCode) {
	std::string	httpVer = HTTP_VERS;
	std::string	httpStatCode = numToStr(statusCode);
	std::string httpStatText = _getStatusText(statusCode);
	return httpVer + " " + httpStatCode + " " + httpStatText + CRLF;
}

std::string	HttpResponse::_getContentLength(void) {
	std::string	contentLength = "Content-Length: ";
	size_t	length = _body.length();
	return contentLength + numToStr(length) + CRLF;
}

std::string	HttpResponse::_getContentType(std::string & path) {
	std::string	contentType = "Content-Type: ";
	size_t	index = path.find_last_of(".");
	if (index != std::string::npos) {
		std::string	ext = path.substr(index + 1);
		return contentType + _req.serv.getMimeType(ext) + CRLF;
	}
	return contentType + _req.serv.getMimeType("default") + CRLF;
}

std::string	HttpResponse::_getDate(void) {
	std::string	date = "Date: ";
	std::time_t	currentTime;
	struct tm	*gmTime;
	char		buffer[30];

	std::time(&currentTime);		// get current time.
	gmTime = gmtime(&currentTime);	// convert to tm struct GMT

	// Date: <day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT
	std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %T %Z", gmTime);
	return date + buffer + CRLF;
}

// ************************************************************************** //
// ----------------------------- Body Messages ------------------------------ //
// ************************************************************************** //

bool	HttpResponse::_readFile(std::string & fileName, std::string & buffer) {
	std::ifstream	inFile;
	int				length;

	inFile.open(fileName.c_str());		// Convert string to char* by c_str() function
	if (!inFile.is_open()) {
		if (errno == ENOENT) {	// 2 No such file or directory : 404
			_status = 404;
			return false;
		}
		if (errno == EACCES) { // 13 Permission denied : 403
			_status = 403;
			return false;
		}
		// EMFILE, ENFILE : Too many open file, File table overflow
		// Server Error, May reach the limit of file descriptors : 500
		// std::cerr << "Internal Server Error" << std::endl;
		_status = 500;
		return (false);
	} // : TODO Check it is a file not a dir if dir are there autoindex
	inFile.seekg(0, inFile.end);		// Set position to end of the stream
	length = inFile.tellg();			// Get current position
	inFile.seekg(0, inFile.beg);		// Set position back to begining of the stream
	buffer.resize(length);
	inFile.read(&buffer[0], length);	// Read all data in inFile to Buffer
	inFile.close();						// Close inFile
	_status = 200;
	return (true);
}

bool	HttpResponse::_isCgi(std::string & path) {
	size_t	index = path.find_last_of(".");
	if (index != std::string::npos) {
		std::string	ext = path.substr(index + 1);
		if (ext == "sh")
			return true;
	}
	return false;
}

std::string	HttpResponse::_getStatusText(short int & statusCode) {
	switch (statusCode)
	{
	case 200:
		return "OK";
	case 403:
		return "Forbidden";
	case 404:
		return "Not Found";
	case 405:
		return "Method Not Allowed";
	case 505:
		return "HTTP Version Not Supported";
	default:
		return "Undefined";
	}
}

// ************************************************************************** //
// ---------------------------- Parsing Request ----------------------------- //
// ************************************************************************** //

// Finding special header in map headers
std::string	HttpResponse::_findContent(std::map<std::string, std::string> & map, std::string const & content) {
	std::map<std::string, std::string>::const_iterator	it;
	std::string	value = "";

	it = map.find(content);
	if (it != map.end()) {
		value = it->second;
		map.erase(it);
	}
	return value;
}

// Parsing URI to each path
bool	HttpResponse::_splitPath(std::string url) {
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
	_req.path = url;
	return true;
}

void	HttpResponse::prtParsedReq(void) {
	std::cout <<  "--- Parsed Request ---" << std::endl;
	std::cout << "cliIPaddr: " << PURPLE << _req.cliIPaddr << RESET << std::endl;
	std::cout << "method: " << PURPLE << _req.method << RESET << std::endl;
	std::cout << "uri: " << PURPLE << _req.uri << RESET << std::endl;
	std::cout << "version: " << PURPLE << _req.version << RESET << std::endl;
	std::cout << "contentLengt: " << PURPLE << _req.contentLength << RESET << std::endl;
	std::cout << "contentType: " << PURPLE << _req.contentType << RESET << std::endl;
	std::cout << "path: " << PURPLE << _req.path << RESET << std::endl;
	std::cout << "pathInfo: " << PURPLE << _req.pathInfo << RESET << std::endl;
	std::cout << "queryStr: " << PURPLE << _req.queryStr << RESET << std::endl;
	std::cout << "fragment: " << PURPLE << _req.fragment << RESET << std::endl;
	std::cout << "body: " << PURPLE << _req.body << RESET << std::endl;
	prtMap(_req.headers);
	std::cout <<  "--- Parsed Server ---" << std::endl;
	_req.serv.prtServer();
}

bool	HttpResponse::_findFile(void) {
	struct stat	fileInfo;
	std::string	path;

	path = _req.serv.getRoot();
	if (_req.path != "/")
		path += _req.path;
	if (stat(path.c_str(), &fileInfo) != 0) {
		std::cerr << "Path not real" << std::endl;
		return false;
	}
	if (S_ISREG(fileInfo.st_mode)) { // is regular file
		// std::cout << RED << "is reg" << RESET << std::endl;
		_req.path = path;
	}
	else if (S_ISDIR(fileInfo.st_mode)) { // is directory
		// std::cout << RED << "is dir" << RESET << std::endl;
		std::vector<std::string>	index = _req.serv.getIndex();
		std::string	filePath;
		for (int i = 0; i < index.size(); i++) {
			filePath = path + "/" + index[i];
			if (stat(filePath.c_str(), &fileInfo) != 0)
				continue;
			if (S_ISREG(fileInfo.st_mode)) {
				_req.path = filePath;
				break;
			}
		}
	}
	else {
		std::cerr << "Not a file" << std::endl;
		return false;
	}
	std::cout << "Find path success" << std::endl;
	std::cout << "path : " << PURPLE << _req.path << RESET << std::endl;
	return true;
}

// int	HttpResponse::_matchLocation(void) {
// 	int	i;

// 	i = _mathServer(req, servs);
// }
