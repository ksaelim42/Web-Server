#include "HttpResponse.hpp"

std::string	HttpResponse::getContentType(std::string path) {
	std::string	extension = path.find_last_of(".")
}