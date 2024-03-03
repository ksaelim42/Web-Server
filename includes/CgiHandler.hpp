#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# define CGI_VERS		"CGI/1.0"
# define PROGRAM_NAME	"MPM/1.0"

#include <fstream>
#include <unistd.h>
#include <sys/stat.h>	// stat

#include "httpReq.hpp"
#include "Utils.hpp"
#include "Server.hpp"
#include "Struct.hpp"

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
	std::string							fragment;
	std::string							redirPath;
	std::string							pathSrc;
	std::string							body;
	Server								serv;
};


class CgiHandler
{
	private:
		pid_t		_pid;
		int			_pipeInFd[2];
		int			_pipeOutFd[2];
		short int	_status;
		bool		_isPost;
		std::map<std::string, std::string>	_env;

		// char**	_args; // store CGI script path to execute
		bool	_initEnv(parsedReq &);
		bool	_checkCgiScript(parsedReq &);
		bool	_createPipe(parsedReq &);
		bool	_forkChild(parsedReq &);
		void _childProcess(parsedReq &);
		void _parentProcess(parsedReq &, std::string &);
	public:
		CgiHandler(void);
		// ~Cgi(void);
		short int	execCgiScript(parsedReq &, std::string &);
};

#endif