#ifndef CGI_HPP
# define CGI_HPP

#include <iostream>
#include <string>
#include <unistd.h>

struct t_pipe
{
	pid_t	pid;
	int		fd[2];
	int		infd;
	int		outfd;
	int		pipeout;
};

int	handlerCgi(std::string);

#endif