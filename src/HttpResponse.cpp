#include "../include/HttpResponse.hpp"

HttpResponse::HttpResponse(Server & server, request_t & request)
 : _server(server), _request(request) {}

std::string	HttpResponse::createResponse(void) {
	readFile(_request.path, _body);
	_contentLength = getContentLength();
	_contentType = getContentType(_request.path, _server.mimeType);
	_date = getDate();
	_header = getStatusLine(_statusCode);
	_header += _contentLength;
	_header += _contentType;
	_header += _date;
	return _header + CRLF + _body;
}

void	HttpResponse::prtResponse(void) {
	std::cout << _contentType;
	std::cout << _contentLength;
	std::cout << _date;
	std::cout << "-----------------------------------------" << std::endl;
	std::cout << _body;
}

std::string	HttpResponse::getStatusLine(short int & statusCode) {
	std::string	httpVer = HTTPVERS;
	std::string	httpStatCode = itoa(statusCode);
	std::string httpStatText = getStatusText(statusCode);
	return httpVer + " " + httpStatCode + " " + httpStatText + CRLF;
}

std::string	HttpResponse::getContentLength(void) {
	std::string	contentLength = "Content-Length: ";
	size_t	length = _body.length();
	return contentLength + itoa(length) + CRLF;
}

std::string	HttpResponse::getContentType(std::string & path, std::map<std::string, std::string> & mimeType) {
	std::string	contentType = "Content-Type: ";
	size_t	index = path.find_last_of(".");
	if (index != std::string::npos) {
		std::string	ext = path.substr(index + 1);
		if (mimeType.count(ext))
			return contentType + mimeType[ext] + CRLF;
	}
	return contentType + mimeType["default"] + CRLF;
}

std::string	HttpResponse::getDate(void) {
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

bool	HttpResponse::readFile(std::string & fileName, std::string & buffer) {
	std::ifstream	inFile;
	int				length;

	inFile.open(fileName.c_str());		// Convert string to char* by c_str() function
	if (!inFile.is_open()) {
		if (errno == ENOENT) {	// 2 No such file or directory : 404
			_statusCode = 404;
			return false;
		}
		if (errno == EACCES) { // 13 Permission denied : 403
			_statusCode = 403;
			return false;
		}
		// EMFILE, ENFILE : Too many open file, File table overflow
		// Server Error, May reach the limit of file descriptors : 500
		// std::cerr << "Internal Server Error" << std::endl;
		_statusCode = 500;
		return (false);
	} // : TODO Check it is a file not a dir if dir are there autoindex
	inFile.seekg(0, inFile.end);		// Set position to end of the stream
	length = inFile.tellg();			// Get current position
	inFile.seekg(0, inFile.beg);		// Set position back to begining of the stream
	buffer.resize(length);
	inFile.read(&buffer[0], length);	// Read all data in inFile to Buffer
	inFile.close();						// Close inFile
	_statusCode = 500;
	return (true);
}

std::string	HttpResponse::getStatusText(short int & statusCode) {
	switch (statusCode)
	{
	case 200:
		return "OK";
	case 403:
		return "Forbidden";
	case 404:
		return "Not Found";
	default:
		return "Undefined";
	}
}
