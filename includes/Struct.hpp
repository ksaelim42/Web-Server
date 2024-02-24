#ifndef STRUCT_HPP
# define STRUCT_HPP

#include <string>
#include <vector>

struct return_t {
	short int	code;	// Status Code
	std::string	text;	// Option
};

struct method_t {
	bool	get;
	bool	head;
	bool	post;
	bool	del;
};

struct Location
{
	std::string					path;	// path
	std::string					root;
	std::vector<std::string>	index;
	uint16_t					allowMethod;
	bool						autoIndex;
	size_t						cliBodySize;
	return_t					retur;
	bool						cgiPass;
};


#endif