#include "header.hpp"

bool	CgiHandler::execute(std::string path) {
	_isPost = 0;
	if (!_createPipe())
		return _prtErr("[CGI] - Error for create Pipe"), false;
	if (!_setNonBlocking())
		return _prtErr("[CGI] - Error for setting Non-Blocking I/O"), false;
	_pid = fork();
	if (_pid == -1)
		return _prtErr("[CGI] - Error for fork child"), false;
	if (_pid == 0)
		_childProcess(path);
	else {
		std::cout << YEL << "[CGI] - Start pid: " << _pid << RESET << std::endl;
		_parentProcess();
	}
	return true;
}

void	CgiHandler::_closePipe(int &fd) {
	close(fd);
	fd = -1;
}

void	CgiHandler::_closeAllPipe(void) {
	if (_isPost) {
		_closePipe(_pipeInFd[0]);
		_closePipe(_pipeInFd[1]);
	}
	_closePipe(_pipeOutFd[0]);
	_closePipe(_pipeOutFd[1]);
}

bool	CgiHandler::_gotoCgiDir(std::string srcPath, std::string & fileCgi) {
	std::size_t	found = srcPath.find_last_of("/");
	if (found != std::string::npos) {
		std::string	path = srcPath.substr(0, found);
		fileCgi = srcPath.substr(found + 1);
		if (chdir(path.c_str()) == -1)
			return false;
	}
	else
		fileCgi = srcPath;
	return true;
}

bool	CgiHandler::_createPipe(void) {
	if (pipe(_pipeOutFd) == -1) { // Sent output from Child to Parent
		return false;
	}
	if (_isPost) {
		if (pipe(_pipeInFd) == -1) { // Sent input from Parent to Child
			close(_pipeOutFd[0]);
			close(_pipeOutFd[1]);
			return false;
		}
	}
	return true;
}

bool	CgiHandler::_setNonBlocking(void) {
	if (fcntl(_pipeOutFd[0], F_SETFL, O_NONBLOCK) < 0)
		return false;
	if (fcntl(_pipeOutFd[1], F_SETFL, O_NONBLOCK) < 0)
		return false;
	if (_isPost) {
		if (fcntl(_pipeInFd[0], F_SETFL, O_NONBLOCK) < 0)
			return false;
		if (fcntl(_pipeInFd[1], F_SETFL, O_NONBLOCK) < 0)
			return false;
	}
	return true;
}

void	CgiHandler::_prtErr(std::string msg) {
	std::cout << RED << msg << RESET << std::endl;
}

void	CgiHandler::_prtMsg(std::string msg) {
	std::cout << YEL << msg << RESET << std::endl;
}

void	CgiHandler::_childProcess(std::string path) {
	std::string	fileCgi;
	if (_isPost) {
		dup2(_pipeInFd[0], STDIN_FILENO);
		_closePipe(_pipeInFd[1]);
		_closePipe(_pipeInFd[0]);
	}
	if (!_gotoCgiDir(path, fileCgi))
		exit(errno);
	dup2(_pipeOutFd[1], STDOUT_FILENO);
	_closePipe(_pipeOutFd[0]);
	_closePipe(_pipeOutFd[1]);
	char *args[3];
	args[0] = strdup("/usr/bin/python3");
	args[1] = strdup(fileCgi.c_str());
	args[2] = NULL;
	// sleep(2); // test
	if (execve(args[0], args, NULL) == -1)
		exit(errno);
}

bool	CgiHandler::_parentProcess(void) {
	int		WaitStat = 5;
	ssize_t	bytesRead;
	char	buffer[100000];
	int		status;

	_closePipe(_pipeOutFd[1]);
	usleep(1000);
	// waitpid(_pid, &WaitStat, 0);
	while (true) {
		// sleep(1);
		status = waitpid(_pid, &WaitStat, WNOHANG);
		std::cout << "pid: " << _pid << ", status: " << status << std::endl;
		std::cout << YEL << "[CGI] - Pid Status: " << WaitStat << RESET << std::endl;
		if (WaitStat != 0)
			return (_prtErr("Wait error"), false);
		if (status == 0)
			continue;
		std::cout << "reading..." << std::endl;
		bytesRead = read(_pipeOutFd[0], buffer, 10000 - 1);
		std::cout << bytesRead << std::endl;
		if (bytesRead == 0)
			break;
		else if (bytesRead < 0)
			return (_prtErr("Read Error"), false);
		std::cout << YEL << "[CGI] - Receive Data form Cgi-script: " << bytesRead << " Bytes" << RESET << std::endl;
		buffer[bytesRead] = '\0';
		std::cout << buffer << std::endl;
	}
	_closePipe(_pipeOutFd[0]);
	return true;
}

