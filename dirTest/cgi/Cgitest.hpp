#ifndef CGITEST_HPP
# define CGITEST_HPP

#include <unistd.h>		// pipe, fork, read, write
#include <fcntl.h>		// check fd is open
#include <sys/wait.h>	// waitpid
#include <cerrno>		// errno
#include <cstdlib>		// exit
#include <cstring>
#include <string>
#include <vector>
#include <iostream>

# define RED	"\e[0;31m"
# define GRN	"\e[0;32m"
# define YEL	"\e[0;33m"
# define BLU	"\e[0;34m"
# define MAG	"\e[0;35m"
# define CYN	"\e[0;36m"
# define WHT	"\e[0;37m"
# define RESET	"\e[0m"
# define CRLF	"\r\n"

# define BBLU	"\e[1;34m"
# define BYEL	"\e[1;33m"

class CgiHandler
{
	private:
		pid_t		_pid;
		int			_pipeInFd[2];
		int			_pipeOutFd[2];
		bool		_isPost;

		bool	_createPipe(void);
		bool	_gotoCgiDir(std::string ,std::string &);
		void	_closeAllPipe(void);
		void	_closePipe(int &fd);
		void	_prtErr(std::string);
		void	_prtMsg(std::string);
		void	_childProcess(std::string path);
		bool	_parentProcess(void);
	public:
		bool	execute(std::string);
		// bool	sendBody(const char *, size_t &, parsedReq &, type_e &);
		// bool	receiveResponse(short int &, std::string &);
		int		getFdIn(void);
		int		getFdOut(void);
};

#endif