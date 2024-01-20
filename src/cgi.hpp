#ifndef CGI_HPP
# define CGI_HPP

#include <iostream>
#include <string>
#include <unistd.h>
#include <string.h>
#include <cstdlib>

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
public:
	// Cgi(/* args */);
	// ~Cgi();
	char**	createArgs(std::string path);
	int		handlerCgi(std::string);
};


#endif