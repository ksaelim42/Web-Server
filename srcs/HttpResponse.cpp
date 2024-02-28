#include "HttpResponse.hpp"

HttpResponse::HttpResponse(Server & serv, httpReq & req) {
	// Request Field
	_req.serv = serv;
	_req.cliIPaddr = "";		// Can't find way
	_req.method = req.method;
	_req.uri = req.srcPath;
	_req.version = req.version;
	_req.headers = req.headers;
	_req.contentLength = _findContent(_req.headers, "Content-Length");
	_req.contentType = _findContent(_req.headers, "Content-Type");
	_parsePath(_req.uri);
	_urlEncoding(_req.path);
	_req.body = ""; // TODO
	_matchLocation(_req.serv.getLocation());
	_req.serv.clearLocation();
	_status = 200; // TODO : must get from prach
	_req.pathSrc = "";
	if (_req.serv.retur.have) {
		_status = _req.serv.retur.code;
		_body = _req.serv.retur.text;
		_req.pathSrc = _req.serv.retur.text;
	}
}

std::string	HttpResponse::createResponse(void) {
	if (_req.serv.retur.have == 0) {
		if (_status == 200)
			_status = _checkRequest();
		if (_status == 200)
			_status = _findFile();
		if (_status == 200)
			_status = _findType();
		if (_status == 200) {
			std::cout << "cgi Pass:   " << _req.serv.cgiPass << std::endl;
			std::cout << "path info : " << _req.pathInfo << std::endl;
			if (_req.serv.cgiPass)
				_status = _cgi.execCgiScript(_req, _body);
			else if (_req.serv.autoIndex == 1 && S_ISDIR(_fileInfo.st_mode))
				_status = _listFile(_req.path, _body);
			else
				_status = _readFile(_req.pathSrc, _body);
		}
		std::cout << RED << "Success for readfile : " << _status << std::endl;
		if (_status >= 400 && _status < 600)
			_createErrorPage(_status, _body);
	}
	return _createHeader() + CRLF + _body;
}

std::string	HttpResponse::_createHeader(void) {
	std::string	headerMsg;
	std::map<std::string, std::string>::const_iterator	it;

	headerMsg.reserve(HEADBUFSIZE);
	headerMsg = _getStatusLine(_status);
	if (_headers.count("Content-Length") == 0)
		_headers["Content-Length"] = _getContentLength();
	if (_headers.count("Content-Type") == 0)
		_headers["Content-Type"] = _getContentType(_req.pathSrc);
	if (_headers.count("Date") == 0)
		_headers["Date"] = _getDate();
	if (_status >= 300 && _status < 400) {
		if (_headers.count("Location") == 0)
		_headers["Location"] = _getLocation(_req.path);
	}
	size_t	i = 0;
	for (it = _headers.begin(); it != _headers.end(); it++)
		headerMsg += it->first + ":" + it->second + CRLF;
	return headerMsg;
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

short int	HttpResponse::_checkRequest(void) {
	if (!_checkMethod(_req.method))
		return 405;
	if (!_checkVersion(_req.version))
		return 505;
	return 200;
}

bool	HttpResponse::_checkMethod(std::string method) {
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
	return numToStr(_body.length());
}

std::string	HttpResponse::_getContentType(std::string & path) {
	size_t	index = path.find_last_of(".");
	if (index != std::string::npos) {
		std::string	ext = path.substr(index + 1);
		return _req.serv.getMimeType(ext);
	}
	return _req.serv.getMimeType("default");
}

std::string	HttpResponse::_getDate(void) {
	std::time_t	currentTime;
	struct tm	*gmTime;
	char		buffer[30];

	std::time(&currentTime);		// get current time.
	gmTime = gmtime(&currentTime);	// convert to tm struct GMT

	// Date: <day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT
	std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %T %Z", gmTime);
	return buffer;
}

std::string	HttpResponse::_getStatusText(short int & statusCode) {
	switch (statusCode) {
		case 200:
			return "OK";
		case 301:
			return "Moved Permanently";
		case 302:
			return "Found";
		case 403:
			return "Forbidden";
		case 404:
			return "Not Found";
		case 405:
			return "Method Not Allowed";
		case 502:
			return "Bad Gateway";
		case 505:
			return "HTTP Version Not Supported";
		default:
			return "Undefined";
	}
}

std::string	HttpResponse::_getLocation(std::string & url) {
	std::string	location = "http://";

	std::cout << "url :    " << url << std::endl;
	std::map<std::string, std::string>::const_iterator	it;
	it = _req.headers.find("Host");
	if (it == _req.headers.end())
		return "";
	return location + it->second + url + "/";
}

// ************************************************************************** //
// ----------------------------- Body Messages ------------------------------ //
// ************************************************************************** //

short int	HttpResponse::_readFile(std::string & fileName, std::string & body) {
	std::ifstream	inFile;
	int				length;

	inFile.open(fileName.c_str());		// Convert string to char* by c_str() function
	if (!inFile.is_open()) {
		std::cout << "What wrong :" << fileName << std::endl;
		if (errno == ENOENT)	// 2 No such file or directory : 404
			return 404;
		if (errno == EACCES)	// 13 Permission denied : 403
			return 403;
		// EMFILE, ENFILE : Too many open file, File table overflow
		// Server Error, May reach the limit of file descriptors : 500
		// std::cerr << "Internal Server Error" << std::endl;
		return 500;
	} // : TODO Check it is a file not a dir if dir are there autoindex
	inFile.seekg(0, inFile.end);		// Set position to end of the stream
	length = inFile.tellg();			// Get current position
	inFile.seekg(0, inFile.beg);		// Set position back to begining of the stream
	body.resize(length);
	inFile.read(&body[0], length);	// Read all data in inFile to Buffer
	inFile.close();						// Close inFile
	return 200;
}

// bool	HttpResponse::_isCgi(std::string & path) {
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

bool	HttpResponse::_createErrorPage(short int & status, std::string & bodyMsg) {
	_req.pathSrc = _req.serv.getErrPage(status);
	if (_req.pathSrc.empty())
		bodyMsg = "Something went wrong";
	else {
		if (_readFile(_req.pathSrc, bodyMsg) != 200) {
			std::cout << "read err file" << std::endl;
			bodyMsg = "Something went wrong";
		}
		else
			std::cout << "read err file success" << std::endl;
	}
	return true;
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

// Percent encode
bool	HttpResponse::_urlEncoding(std::string & path) {
	std::size_t	found;

	found = path.find_first_of("%");	// return index of found character
	while (found != std::string::npos) {
		if (found + 2 >= path.length())	// if not follow with 2 character
			return false;				// 400 : bad request
		if (!std::isxdigit(path[found + 1]) || !std::isxdigit(path[found + 2]))
			return false;
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
bool	HttpResponse::_parsePath(std::string url) {
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
// bool	HttpResponse::_isCgi(std::string & path) {
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

bool	HttpResponse::_matchLocation(std::vector<Location> loc) {
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
		return true;
	}
	else
		return false;
}

short int	HttpResponse::_findFile(void) {
	std::string	path;

	path = _req.serv.getRoot() + _req.path;
	std::cout << "path : " << path << std::endl;
	std::cout << "reqpath : " << _req.path << std::endl;
	if (path.back() == '/') {
		std::vector<std::string>	index = _req.serv.getIndex();
		std::string	filePath;
		for (size_t	i = 0; i < index.size(); i++) {
			filePath = path + index[i];
			if (stat(filePath.c_str(), &_fileInfo) == 0) {
				_req.pathSrc = filePath;
				std::cout << "Find path success, path: " << PURPLE << _req.pathSrc << RESET << std::endl;
				return 200;
			}
		}
	}
	if (stat(path.c_str(), &_fileInfo) == 0) {
		_req.pathSrc = path;
		std::cout << "Find path success, path: " << PURPLE << _req.pathSrc << RESET << std::endl;
		return 200;
	}
	std::cout << RED << "Not found file in stat" << RESET << std::endl;
	return 404;
}

short int	HttpResponse::_findType(void) {
	if (S_ISDIR(_fileInfo.st_mode)) { // is directory
		if (_req.path.back() == '/') {
			if (_req.serv.autoIndex)
				return 200;
			else
				return 403;
		}
		else
			return 301;
	}
	else if (S_ISREG(_fileInfo.st_mode))
		return 200;
	return 404;
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

short int	HttpResponse::_listFile(std::string & path, std::string & body) {
	DIR					*dir;
	struct dirent		*entry;
	struct stat			fileStat;
	struct tm			*gmTime;
	char				buffer[20];

	body += "<!DOCTYPE html>\n";
	body += "<html>\n";
	body += "<head>\n";
	body += "<title>Index of " + path +" </title>\n";
	body += "</head>\n";
	body += "<body>\n";
	body += "<h1>Index of " + path + " </h1>\n"; // :TODO fix name
	body += "<hr>\n";
	body += "<pre>\n";
	dir = opendir(_req.pathSrc.c_str());
	if (dir == nullptr) {
		std::cerr << "Error opening directory." << std::endl;
		return 0;
	}
	while ((entry = readdir(dir)) != nullptr) {
		std::string	fileName(entry->d_name);
		if (fileName == ".")
			continue ;
		if (fileName == "..") {
			body += "<a href=\"../\">../</a>\n";
			continue ;
		}
		std::string filePath = _req.pathSrc + fileName;
		if (stat(filePath.c_str(), &fileStat) == 0) {
			std::string	line = "<a href=\"";
			if (S_ISDIR(fileStat.st_mode))
				fileName += "/";
			line += fileName + "\">";
			memset(buffer, 0, 20);
			std::strftime(buffer, sizeof(buffer), "%d-%b-%Y %R", gmtime(&fileStat.st_mtime));
			if (fileName.length() > 40)
				fileName = fileName.substr(0, 37) + "..>";
			std::ostringstream	ss;
			ss << std::setw(44) << std::left << fileName + "</a>";
			ss << std::setw(20) << std::right << buffer;
			if (S_ISDIR(fileStat.st_mode))
				ss << std::setw(20) << std::right << "-" << std::endl;
			else
				ss << std::setw(20) << std::right << fileStat.st_size << std::endl;
			line += ss.str();
			body += line;
		}
	}
	closedir(dir);
	body += "</pre>\n";
	body += "<hr>\n";
	body += "</body>\n";
	body += "</html>";
	_req.pathSrc = "list.html";
	return 200;
}

// ************************************************************************** //
// --------------------------- Inspect CGI script --------------------------- //
// ************************************************************************** //

short int	HttpResponse::_inspectCgiHeaders(std::string & cgiMsg) {
	std::size_t	found  = cgiMsg.find_first_of("\n");

	while (found != std::string::npos) {
		std::string	header(strCutTo(cgiMsg, "\n"));
		std::string	key(strCutTo(header, ":"));
		if (key.find(" ") != std::string::npos)
			return 502;
		_headers[toProperCase(key)] = header;
		found = cgiMsg.find_first_of("\n");
	}
	return 200;
}

// std::string	HttpResponse::_inspectCgiResponse(std::string & cgiMsg) {
// 	std::size_t	found;

// 	for (found = cgiMsg.find_first_of("\n"); found != std::string::npos;) {

// 		found = cgiMsg.find_first_of("\n")
// 	}

// 	}
// 	if (found != std::string::npos) {
// 		_req.fragment = url.substr(found + 1);
// 		url = url.substr(0, found);
// 	}

// 	return httpVer + " " + httpStatCode + " " + httpStatText + CRLF;
// }

// httpReq	genRequest(std::string str) {
// 	// Response Field
// 	_header = "";
// 	_body = "";
// 	_contentType = "";
// 	_contentLength = "";
// 	_connection = "";
// 	_location = "";
// 	_date = "";
// 	httpReq	req;


// 	req.method = strCutTo(str, " ");
// 	req.srcPath = strCutTo(str, " ");
// 	req.version = strCutTo(str, CRLF);
// 	while (str.compare(0, 2, CRLF) != 0) {
// 		std::string key = strCutTo(str, ": ");
// 		std::string value = strCutTo(str, CRLF);;
// 		// std::cout << "key: " << key << std::endl;
// 		// std::cout << "value: " << value << std::endl;
// 		req.headers[key] = value;
// 		// sleep(1);
// 	}
// 	req.body = "";
// 	return req;
// }

// std::string	HttpResponse::_inspectCgiHeader(short int & statusCode) {
// 	std::string	httpVer = HTTP_VERS;
// 	std::string	httpStatCode = numToStr(statusCode);
// 	std::string httpStatText = _getStatusText(statusCode);
// 	return httpVer + " " + httpStatCode + " " + httpStatText + CRLF;
// }






// back up
// std::string	HttpResponse::_getContentLength(void) {
// 	std::string	contentLength = "Content-Length: ";
// 	size_t	length = _body.length();
// 	return contentLength + numToStr(length) + CRLF;
// }

// std::string	HttpResponse::_getContentType(std::string & path) {
// 	std::string	contentType = "Content-Type: ";
// 	size_t	index = path.find_last_of(".");
// 	if (index != std::string::npos) {
// 		std::string	ext = path.substr(index + 1);
// 		return contentType + _req.serv.getMimeType(ext) + CRLF;
// 	}
// 	return contentType + _req.serv.getMimeType("default") + CRLF;
// }

// std::string	HttpResponse::_getDate(void) {
// 	std::string	date = "Date: ";
// 	std::time_t	currentTime;
// 	struct tm	*gmTime;
// 	char		buffer[30];

// 	std::time(&currentTime);		// get current time.
// 	gmTime = gmtime(&currentTime);	// convert to tm struct GMT

// 	// Date: <day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT
// 	std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %T %Z", gmTime);
// 	return date + buffer + CRLF;
// }