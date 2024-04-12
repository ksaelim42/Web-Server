#include "header.hpp"

bool	CgiHandler::execute(std::string path) {
	std::cout << YEL << "[CGI] - Start" << RESET << std::endl;
	_isPost = 0;
	if (!_createPipe())
		return _prtErr("[CGI] - Error for create Pipe"), false;
	_pid = fork();
	if (_pid == -1)
		return _prtErr("[CGI] - Error for fork child"), false;
	if (_pid == 0)
		_childProcess(path);
	else
		_parentProcess();
	return true;
}

// bool	CgiHandler::sendBody(const char * body, size_t & bufSize, parsedReq & req, type_e & type) {
// 	size_t	bytes;

// 	if (bufSize) {
// 		bytes = write(_pipeInFd[1], body, bufSize);
// 		if (bytes < bufSize)
// 			return false;
// 		_package++;
// 	}
// 	if (type == CHUNK) {
// 		if (bufSize == 0) {
// 			Logger::isLog(DEBUG) && Logger::log(YEL, "[CGI] - Success for sent pagekage -----");
// 			_closePipe(_pipeInFd[1]);
// 			type = RESPONSE;
// 		}
// 		Logger::isLog(WARNING) && Logger::log(YEL, "[CGI] - chunk[", _package, "] sent ", bufSize, " Bytes");
// 		req.body.clear();
// 		req.bodySize = 0;
// 	}
// 	else {
// 		req.bodySent += bufSize;
// 		Logger::isLog(WARNING) && Logger::log(YEL, "[CGI] - pakage[", _package, "] sent ", req.bodySent, " out of ", req.bodySize);
// 		if (req.bodySent >= req.bodySize) {
// 			Logger::isLog(DEBUG) && Logger::log(YEL, "[CGI] - Success for sent pagekage -----");
// 			_closePipe(_pipeInFd[1]);
// 			type = RESPONSE;
// 		}
// 	}
// 	return true;
// }

// bool	CgiHandler::receiveResponse(short int & status, std::string & cgiMsg) {
// 	int		WaitStat;
// 	ssize_t	bytesRead;
// 	char	buffer[BUFFERSIZE];

// 	waitpid(_pid, &WaitStat, 0);
// 	_pid = -1;
// 	Logger::isLog(DEBUG) && Logger::log(YEL, "[CGI] - Pid Status: ", WaitStat);
// 	if (WaitStat != 0)
// 		return (status = 502, false);
// 	cgiMsg.clear();
// 	while (true) {
// 		bytesRead = read(_pipeOutFd[0], buffer, BUFFERSIZE - 1);
// 		if (bytesRead == 0)
// 			break;
// 		else if (bytesRead < 0)
// 			return (status = 502, false);
// 		Logger::isLog(WARNING) && Logger::log(YEL, "[CGI] - Receive Data form Cgi-script: ", bytesRead, " Bytes");
// 		buffer[bytesRead] = '\0';
// 		cgiMsg += buffer;
// 	}
// 	_closePipe(_pipeOutFd[0]);
// 	return (status = 200, true);
// }

// ************************************************************************** //
// ----------------------------- CGI Processes ------------------------------ //
// ************************************************************************** //

// CgiHandler::~CgiHandler() {
// 	Logger::isLog(WARNING) && Logger::log(YEL, "[CGI] - Destructor");
// 	if (_pipeInFd[1] > 0 && fcntl(_pipeInFd[1], F_GETFL) != -1) {
// 		_closePipe(_pipeInFd[1]);
// 		Logger::isLog(WARNING) && Logger::log(YEL, "fd: ", _pipeInFd[1], " was closed");
// 	}
// 	if (_pipeOutFd[0] > 0 && fcntl(_pipeOutFd[0], F_GETFL) != -1) {
// 		_closePipe(_pipeOutFd[0]);
// 		Logger::isLog(WARNING) && Logger::log(YEL, "fd: ", _pipeOutFd[0], " was closed");
// 	}
// 	if (_pid != -1) {
// 		kill(_pid, SIGKILL);
// 		Logger::isLog(WARNING) && Logger::log(YEL, _pid, " was killed");
// 	}
// }

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
	fcntl(_pipeOutFd[0], F_SETFL, O_NONBLOCK);
	fcntl(_pipeOutFd[1], F_SETFL, O_NONBLOCK);
		// _prtErr("Non-Blocking i/o fail");
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
		// if (_isPost) {
		// 	_closePipe(_pipeInFd[0]);
		// 	if (type == CHUNK)
		// 		return true;
		// 	_package = 1;
		// 	if (req.body.size()) {
		// 		write(_pipeInFd[1], req.body.c_str(), req.body.size());
		// 		req.bodySent += req.body.size();
		// 		req.body.clear();
		// 		_package++;
		// 		Logger::isLog(WARNING) && Logger::log(YEL, "[CGI] - pakage[", _package, "] sent ", req.bodySent, " out of ", req.bodySize);
		// 	}
		// 	if (req.bodySent >= req.bodySize) {
		// 		Logger::isLog(DEBUG) && Logger::log(YEL, "[CGI] - Success for sent pagekage -----");
		// 		_closePipe(_pipeInFd[1]);
		// 		return type = RESPONSE, true;
		// 	}
		// }
		// else 
		// 	type = RESPONSE;

bool	CgiHandler::_parentProcess(void) {
	int		WaitStat;
	ssize_t	bytesRead;
	char	buffer[100000];

	// if (fcntl(_pipeOutFd[1], F_SETFL, O_NONBLOCK) < 0)
	// 	_prtErr("Non-Blocking i/o fail");
	_closePipe(_pipeOutFd[1]);
	// waitpid(_pid, &WaitStat, 0);
	waitpid(_pid, &WaitStat, WNOHANG);
	sleep(2);
	std::cout << YEL << "[CGI] - Pid Status: " << WaitStat << RESET << std::endl;
	if (WaitStat != 0)
		return (_prtErr("Wait error"), false);
	while (true) {
		std::cout << "reading..." << std::endl;
		bytesRead = read(_pipeOutFd[0], buffer, 10 - 1);
		if (bytesRead == 0)
			break;
		else if (bytesRead < 0)
			return (_prtErr("Read Error"), false);
		std::cout << YEL << "[CGI] - Receive Data form Cgi-script: " << bytesRead << " Bytes" << RESET << std::endl;
		buffer[bytesRead] = '\0';
		std::cout << buffer << std::endl;
		// sleep(2);
	}
	_closePipe(_pipeOutFd[0]);
	return true;
}

