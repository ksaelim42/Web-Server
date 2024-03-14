#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# define CGI_VERS		"CGI/1.0"
# define PROGRAM_NAME	"MPM/1.0" // Mos Prach Mark
# define PYTHON_PATH	"/usr/bin/python3"

#include <unistd.h>		// pipe, fork, read, write
#include <sys/stat.h>	// stat
#include <sys/wait.h>	// waitpid

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
		uint64_t	_contentSize;
		std::string	_cgiFileName;
		std::string	_cgiProgramPath;
		std::map<std::string, std::string>	_env;

		bool	_initEnv(parsedReq &);
		bool	_checkCgiScript(short int &, parsedReq &);
		bool	_createPipe(parsedReq &);
		bool	_gotoCgiDir(std::string &);
		void	_closePipe(void);
		void	_childProcess(parsedReq &);
	public:
		bool	sendRequest(short int &, parsedReq &);
		bool	receiveResponse(short int &, std::string &);
};

#endif