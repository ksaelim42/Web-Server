#include "../include/HttpResponse.hpp"

HttpResponse::HttpResponse(Server & server, request_t & request)
 : _server(server), _request(request) {}

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


std::string	HttpResponse::createResponse(void) {
	_contentType = getContentType(_request.path, _server.mimeType);
	return "Hello";
}

void	HttpResponse::prtResponse(void) {
	std::cout << _contentType << std::endl;
}
