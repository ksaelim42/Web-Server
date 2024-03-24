#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

#include <unistd.h>		// pipe, fork, read, write
#include <fcntl.h>		// check fd is open
#include <sys/stat.h>	// stat
#include <sys/wait.h>	// waitpid
#include <cerrno>		// errno
#include <cstdlib>		// exit

#include "HttpRequest.hpp"
#include "Utils.hpp"
#include "Struct.hpp"

class CgiHandler
{
	private:
		pid_t		_pid;
		int			_pipeInFd[2];
		int			_pipeOutFd[2];
		bool		_isPost;
		size_t		_package;
		std::string	_cgiFileName;
		std::string	_cgiProgramPath;
		std::map<std::string, std::string>	_env;

		void	_initCgi();
		bool	_initEnv(parsedReq &);
		bool	_checkCgiScript(short int &, parsedReq &);
		bool	_createPipe(void);
		bool	_gotoCgiDir(std::string &);
		void	_closePipe(void);
		void	_childProcess(parsedReq &);
	public:
		CgiHandler();
		~CgiHandler();
		bool	sendRequest(short int &, parsedReq &);
		bool	sendBody(const char *, size_t &, parsedReq &);
		bool	receiveResponse(short int &, std::string &);
};

#endif