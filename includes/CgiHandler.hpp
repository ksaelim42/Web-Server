#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

#include <unistd.h>		// pipe, fork, read, write
#include <fcntl.h>		// check fd is open
#include <sys/wait.h>	// waitpid
#include <cerrno>		// errno
#include <cstdlib>		// exit

#include "Utils.hpp"
#include "Struct.hpp"
#include "Client.hpp"

class CgiHandler
{
	private:
		pid_t		_pid;
		int			_pipeInFd[2];
		int			_pipeOutFd[2];
		bool		_isPost;
		std::string	_cgiFileName;
		std::string	_cgiProgramPath;
		std::map<std::string, std::string>	_env;

		void	_initCgi();
		bool	_initEnv(parsedReq &);
		bool	_checkCgiScript(short int &, parsedReq &);
		bool	_createPipe(void);
		bool	_setNonBlocking(void);
		bool	_gotoCgiDir(std::string &);
		void	_closeAllPipe(void);
		void	_closePipe(int (&)[2]);
		void	_childProcess(parsedReq &);
	public:
		CgiHandler();
		~CgiHandler();
		bool	createRequest(Client &);
		bool	sendBody(Client &, int);
		ssize_t	receiveResponse(Client &, int, char*);
};

#endif