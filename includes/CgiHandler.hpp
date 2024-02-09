#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

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

class CgiHandler
{
private:
	// char**	_args; // store CGI script path to execute
	std::map<std::string, std::string>	_env;
public:
	CgiHandler(void);
	// ~Cgi(void);
	// char**	createArgs(std::string path);
	int		execCgiScript(std::string);
};


#endif