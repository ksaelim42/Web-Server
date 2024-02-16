#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# define CGI_VERS		"CGI/1.0"
# define PROGRAM_NAME	"MPM/1.0"

// #include <iostream>
// #include <string>
// #include <string.h>
// #include <cstdlib>
#include <fstream>
#include <unistd.h>

#include "httpReq.hpp"
#include "Utils.hpp"
#include "Server.hpp"

struct parsedReq {
	std::string							cliIPaddr;
	std::string							method;
	std::string							uri;
	std::string							version;
	std::map<std::string, std::string>	headers;
	std::string							contentLength;
	std::string							contentType;
	std::string							path;
	std::string							pathInfo;
	std::string							queryStr;
	std::string							body;
	Server								serv;
};


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
		bool	_createCgiRequest(parsedReq &);
		bool	_createPipe(void);
		bool	_prepareScript(void);
		std::map<std::string, std::string>	_env;
	public:
		CgiHandler(void);
		// ~Cgi(void);
		// char**	createArgs(std::string path);
		std::string	execCgiScript(parsedReq &, short int &);
};


#endif