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
		pid_t	_pid;
		int		_pipeInFd[2];
		int		_pipeOutFd[2];
		std::string	_path; // Tmp

		// char**	_args; // store CGI script path to execute
		bool	_createCgiRequest(Server &, request_t &);
		bool	_createPipe(void);
		bool	_prepareScript(void);
		std::map<std::string, std::string>	_env;
	public:
		CgiHandler(void);
		// ~Cgi(void);
		// char**	createArgs(std::string path);
		std::string	execCgiScript(Server &, request_t &, short int &);
};


#endif