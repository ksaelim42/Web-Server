#ifndef CGI_HPP
# define CGI_HPP

// #include <iostream>
// #include <string>
// #include <string.h>
// #include <cstdlib>
#include "Webserv.hpp"

struct t_pipe
{
	pid_t	pid;
	int		fd[2];
	int		infd;
	int		outfd;
	int		pipeout;
};

class Cgi
{
private:
	// char**	_args; // store CGI script path to execute
	std::map<std::string, std::string>	_env;
public:
	Cgi(void);
	// ~Cgi();
	// char**	createArgs(std::string path);
	int		handlerCgi(std::string);
};


#endif