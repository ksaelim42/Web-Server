#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# define CGI_VERS		"CGI/1.0"
# define PROGRAM_NAME	"MPM/1.0" // Mos Prach Mark

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
		std::map<std::string, std::string>	_env;

		bool	_initEnv(parsedReq &);
		bool	_checkCgiScript(short int &, parsedReq &);
		bool	_createPipe(parsedReq &);
		void	_closePipe(void);
		void	_childProcess(parsedReq &);
	public:
		bool		request(short int &, parsedReq &);
		short int	response(std::string &);
};

#endif