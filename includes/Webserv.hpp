#ifndef WEBSERV_HPP
# define WEBSERV_HPP


// for test
struct request_t {
	std::string	method;
	std::string	path;
	std::string	version;
	std::string	body;
};

#endif