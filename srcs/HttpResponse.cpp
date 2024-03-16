#include "HttpResponse.hpp"

std::string	HttpResponse::deleteResource(short int & status, parsedReq & req) {
	std::cout << MAG << "---------------------- delete -----------------------" << RESET << std::endl;
	_body = "";
	if (access(req.pathSrc.c_str(), F_OK) != 0)
		return status = 404, "";
	if (access(req.pathSrc.c_str(), W_OK) != 0)
		return status = 403, "";
	if (std::remove(req.pathSrc.c_str()) != 0)
		return status = 503, "";
	status = 204;
	return _createHeader(status , req) + CRLF + _body;
}

std::string	HttpResponse::redirection(short int & status, parsedReq & req) {
	std::cout << MAG << "---------------------- redir -----------------------" << RESET << std::endl;
	_body = req.serv.retur.text;
	req.pathSrc = req.serv.retur.text;
	return _createHeader(status , req) + CRLF + _body;
}

std::string	HttpResponse::autoIndex(short int & status, parsedReq & req) {
	std::cout << MAG << "---------------------- Auto index -----------------------" << RESET << std::endl;
	_listFile(req, _body);
	return _createHeader(status , req) + CRLF + _body;
}

std::string	HttpResponse::staticContent(short int & status, parsedReq & req) {
	std::cout << MAG << "---------------------- Tat Tic -----------------------" << RESET << std::endl;
	status = _readFile(req.pathSrc, _body);
	if (status >= 200 && status < 300)
		return _createHeader(status, req) + CRLF + _body;
	return ("");
}

std::string	HttpResponse::cgiResponse(short int & status,  parsedReq & req, std::string & cgiMsg) {
	std::string	cgiHeader;

	if (status != 200)
		return "";
	status = _parseCgiHeader(cgiMsg, cgiHeader);
	if (status != 200)
		return "";
	status = _inspectCgiHeaders(cgiHeader);
	if (status != 200)
		return "";
	_body = cgiMsg;
	return _createHeader(status , req) + CRLF + _body;
}

std::string	HttpResponse::errorPage(short int & status, parsedReq & req) {
	std::cout << MAG << "---------------------- Error Page -----------------------" << RESET << std::endl;
	req.pathSrc = req.serv.getErrPage(status);
	if (req.pathSrc.empty())
		_body = "Something went wrong";
	else {
		if (_readFile(req.pathSrc, _body) != 200) { // TODO : file info size
			std::cout << "read err file" << std::endl;
			_body = "Something went wrong";
		}
		else
			std::cout << "read err file success" << std::endl;
	}
	return _createHeader(status , req) + CRLF + _body;
}

void	HttpResponse::clear(void) {
	_headers.clear();
	_body.clear();
}

// ************************************************************************** //
// ------------------------- Response Header Fields ------------------------- //
// ************************************************************************** //

std::string	HttpResponse::_createHeader(short int & status, parsedReq & req) {
	std::string	headerMsg;
	std::map<std::string, std::string>::const_iterator	it;

	headerMsg.reserve(HEADBUFSIZE);
	headerMsg = _getStatusLine(status);
	if (_headers.count("Accept-Ranges") == 0) // TODO : must be check first
		_headers["Accept-Ranges"] = "bytes";
	if (_headers.count("Connection") == 0) // TODO : must be check first
		_headers["Connection"] = "keep-alive";
	if (_headers.count("Content-Length") == 0)
		_headers["Content-Length"] = _getContentLength();
	if (_headers.count("Content-Type") == 0)
		_headers["Content-Type"] = _getContentType(req);
	if (_headers.count("Date") == 0)
		_headers["Date"] = _getDate();
	if (status >= 300 && status < 400) {
		if (_headers.count("Location") == 0)
		_headers["Location"] = _getLocation(req);
	}
	_headers["Connection"] = CONNETION;
	for (it = _headers.begin(); it != _headers.end(); it++)
		headerMsg += it->first + ":" + it->second + CRLF;
	return headerMsg;
}

std::string	HttpResponse::_getStatusLine(short int & statusCode) {
	std::string	httpVer = HTTP_VERS;
	std::string	httpStatCode = numToStr(statusCode);
	std::string httpStatText = _getStatusText(statusCode);
	return httpVer + " " + httpStatCode + " " + httpStatText + CRLF;
}

std::string	HttpResponse::_getContentLength(void) {
	return numToStr(_body.length());
}

std::string	HttpResponse::_getContentType(parsedReq & req) {
	size_t	index = req.pathSrc.find_last_of(".");
	if (index != std::string::npos) {
		std::string	ext = req.pathSrc.substr(index + 1);
		return req.serv.getMimeType(ext);
	}
	return req.serv.getMimeType("default");
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
		case 201:
			return "Created";
		case 204:
			return "No Content";
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
		case 413:
			return "Request Entity Too Large";
		case 500:
			return "Internal Server Error";
		case 502:
			return "Bad Gateway";
		case 503:
			return "Service Unavailable";
		case 505:
			return "HTTP Version Not Supported";
		default:
			return "Undefined";
	}
}

std::string	HttpResponse::_getLocation(parsedReq & req) {
	std::string	location = "http://";

	std::cout << "url :    " << req.path << std::endl;
	std::map<std::string, std::string>::const_iterator	it;
	it = req.headers.find("Host");
	if (it == req.headers.end())
		return "";
	return location + it->second + req.path + "/";
}

// ************************************************************************** //
// ----------------------------- Body Messages ------------------------------ //
// ************************************************************************** //

short int	HttpResponse::_readFile(std::string & path, std::string & body) {
	int			fd;
	struct stat	fileInfo;

	if (stat(path.c_str(), &fileInfo) == -1)
		return 404;
	fd = open(path.c_str(), O_RDONLY);
	if (fd < 0) {
		if (errno == ENOENT)	// 2 No such file or directory : 404
			return 404;
		if (errno == EACCES)	// 13 Permission denied : 403
			return 403;
		// EMFILE, ENFILE : Too many open file, File table overflow
		// Server Error, May reach the limit of file descriptors : 500
		return 500;
	}
	body.resize(fileInfo.st_size);
	read(fd, &body[0], fileInfo.st_size);	// Read all data in inFile to Buffer
	close(fd);
	return 200;
}

short int	HttpResponse::_listFile(parsedReq & req, std::string & body) {
	DIR				*dir;
	char			buffer[20];
	struct dirent	*entry;
	struct stat		fileStat;
	struct tm		*gmTime;
	std::string		files;
	std::string		directories;

	body += "<!DOCTYPE html>\n";
	body += "<html>\n";
	body += "<head>\n";
	body += "<title>Index of " + req.path +" </title>\n";
	body += "</head>\n";
	body += "<body>\n";
	body += "<h1>Index of " + req.path + " </h1>\n"; // :TODO fix name
	body += "<hr>\n";
	body += "<pre>\n";
	dir = opendir(req.pathSrc.c_str());
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
		std::string filePath = req.pathSrc + fileName;
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
			if (S_ISDIR(fileStat.st_mode))
				directories += line;
			else
				files += line;
		}
	}
	body += directories + files;
	closedir(dir);
	body += "</pre>\n";
	body += "<hr>\n";
	body += "</body>\n";
	body += "</html>";
	req.pathSrc = "list.html";
	return 200;
}

// ************************************************************************** //
// --------------------------- Inspect CGI script --------------------------- //
// ************************************************************************** //

short int	HttpResponse::_parseCgiHeader(std::string & cgiMsg, std::string	& cgiHeadMsg) {
	// std::cout << cgiMsg << std::endl; // debug
	std::size_t	found  = cgiMsg.find("\n\n");
	if (found == std::string::npos)
		return 502;
	cgiHeadMsg = cgiMsg.substr(0, found + 1);	// Cut the second '\n' out
	cgiMsg.erase(0, found + 2);					// Kept only body Message
	return 200;
}

short int	HttpResponse::_inspectCgiHeaders(std::string & cgiHeadMsg) {
	std::size_t	found  = cgiHeadMsg.find_first_of("\n");

	while (found != std::string::npos) {
		std::string	header(strCutTo(cgiHeadMsg, "\n"));
		std::string	key(strCutTo(header, ":"));
		if (key.find(" ") != std::string::npos)
			return 502;
		_headers[toProperCase(key)] = header;
		found = cgiHeadMsg.find_first_of("\n");
	}
	std::cout << "Inspect CGI heasers success" << std::endl;
	std::cout << "header size: " << _headers.size() << std::endl;
	return 200;
}

void	HttpResponse::prtResponse(void) {
	std::cout << CYN;
	prtMap(_headers);
	std::cout << "-----------------------------------------" << std::endl;
	std::cout << _body << RESET << std::endl;
}
