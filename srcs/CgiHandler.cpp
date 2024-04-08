#include "CgiHandler.hpp"

CgiHandler::CgiHandler() {
	_initCgi();
}

CgiHandler::~CgiHandler() {
	Logger::isLog(WARNING) && Logger::log(YEL, "[CGI] - Destructor");
	if (_pipeInFd[1] > 0 && fcntl(_pipeInFd[1], F_GETFL) != -1) {
		_closePipe(_pipeInFd[1]);
		Logger::isLog(WARNING) && Logger::log(YEL, "fd: ", _pipeInFd[1], " was closed");
	}
	if (_pipeOutFd[0] > 0 && fcntl(_pipeOutFd[0], F_GETFL) != -1) {
		_closePipe(_pipeOutFd[0]);
		Logger::isLog(WARNING) && Logger::log(YEL, "fd: ", _pipeOutFd[0], " was closed");
	}
	if (_pid != -1) {
		kill(_pid, SIGKILL);
		Logger::isLog(WARNING) && Logger::log(YEL, _pid, " was killed");
	}
}

void	CgiHandler::_initCgi() {
	_pid = -1;
	_isPost = 0;
	_package = 1;
	_pipeInFd[0] = -1;
	_pipeInFd[1] = -1;
	_pipeOutFd[0] = -1;
	_pipeOutFd[1] = -1;
	_cgiFileName = "";
	_cgiProgramPath = "";
	_env.clear();
}

bool	CgiHandler::sendRequest(short int & status, parsedReq & req, type_e & type) {
	Logger::isLog(DEBUG) && Logger::log(YEL, "[CGI] - Start");
	_initCgi();
	if (_checkCgiScript(status, req) == 0)
		return false;
	if (!_initEnv(req))
		return false;
	if (!_createPipe()) {
		Logger::isLog(DEBUG) && Logger::log(RED, "[CGI] - Error for create Pipe");
		return (status = 500, false);
	}
	_pid = fork();
	if (_pid == -1) {
		Logger::isLog(DEBUG) && Logger::log(RED, "[CGI] - Error for fork child");
		return (_closeAllPipe(), status = 500, false);
	}
	if (_pid == 0) // Child Process
		_childProcess(req);
	else { // Parent Process
		_closePipe(_pipeOutFd[1]);
		if (_isPost) {
			_closePipe(_pipeInFd[0]);
			if (type == CHUNK)
				return true;
			_package = 1;
			if (req.body.size()) {
				write(_pipeInFd[1], req.body.c_str(), req.body.size());
				req.bodySent += req.body.size();
				req.body.clear();
				_package++;
				Logger::isLog(WARNING) && Logger::log(YEL, "[CGI] - pakage[", _package, "] sent ", req.bodySent, " out of ", req.bodySize);
			}
			if (req.bodySent >= req.bodySize) {
				Logger::isLog(DEBUG) && Logger::log(YEL, "[CGI] - Success for sent pagekage -----");
				_closePipe(_pipeInFd[1]);
				return type = RESPONSE, true;
			}
		}
		else 
			type = RESPONSE;
	}
	return true;
}

bool	CgiHandler::sendBody(const char * body, size_t & bufSize, parsedReq & req, type_e & type) {
	size_t	bytes;

	if (bufSize) {
		bytes = write(_pipeInFd[1], body, bufSize);
		if (bytes < bufSize)
			return false;
		_package++;
	}
	if (type == CHUNK) {
		if (bufSize == 0) {
			Logger::isLog(DEBUG) && Logger::log(YEL, "[CGI] - Success for sent pagekage -----");
			_closePipe(_pipeInFd[1]);
			type = RESPONSE;
		}
		Logger::isLog(WARNING) && Logger::log(YEL, "[CGI] - chunk[", _package, "] sent ", bufSize, " Bytes");
		req.body.clear();
		req.bodySize = 0;
	}
	else {
		req.bodySent += bufSize;
		Logger::isLog(WARNING) && Logger::log(YEL, "[CGI] - pakage[", _package, "] sent ", req.bodySent, " out of ", req.bodySize);
		if (req.bodySent >= req.bodySize) {
			Logger::isLog(DEBUG) && Logger::log(YEL, "[CGI] - Success for sent pagekage -----");
			_closePipe(_pipeInFd[1]);
			type = RESPONSE;
		}
	}
	return true;
}

bool	CgiHandler::receiveResponse(short int & status, std::string & cgiMsg) {
	int		WaitStat;
	ssize_t	bytesRead;
	char	buffer[BUFFERSIZE];

	waitpid(_pid, &WaitStat, 0);
	_pid = -1;
	Logger::isLog(DEBUG) && Logger::log(YEL, "[CGI] - Pid Status: ", WaitStat);
	if (WaitStat != 0)
		return (status = 502, false);
	cgiMsg.clear();
	while (true) {
		bytesRead = read(_pipeOutFd[0], buffer, BUFFERSIZE - 1);
		if (bytesRead == 0)
			break;
		else if (bytesRead < 0)
			return (status = 502, false);
		Logger::isLog(WARNING) && Logger::log(YEL, "[CGI] - Receive Data form Cgi-script: ", bytesRead, " Bytes");
		buffer[bytesRead] = '\0';
		cgiMsg += buffer;
	}
	_closePipe(_pipeOutFd[0]);
	return (status = 200, true);
}

int	CgiHandler::getFdIn(void) {
	return _pipeInFd[1];
}

int	CgiHandler::getFdOut(void) {
	return _pipeOutFd[0];
}

// ************************************************************************** //
// ----------------------------- CGI Processes ------------------------------ //
// ************************************************************************** //

bool	CgiHandler::_initEnv(parsedReq & req) {
	_env.clear();
	// meta-variable
	_env["GATEWAY_INTERFACE"] = CGI_VERS;		// version of CGI
	_env["SERVER_SOFTWARE"] = PROGRAM_NAME;		// name of Webserv/version Ex: nginx/1.18.0
	_env["REQUEST_METHOD"] = req.method;		// HTTP method Ex: GET
	_env["REQUEST_URI"] = req.uri;				// URI (not encode URL)
	_env["SERVER_PROTOCOL"] = req.version;		// HTTP version that get from request (Server must check that support) Ex : HTTP/1.1
	_env["CONTENT_LENGTH"] = findHeaderValue(req.headers, "Content-Length");	// Must specify on POST method for read body content
	_env["CONTENT_TYPE"] = findHeaderValue(req.headers, "Content-Type");		// get from request
	// Parsing
	_env["SCRIPT_NAME"] = req.pathSrc;			// path of script (exclude Query string & path info) Ex: /script.sh
	_env["QUERY_STRING"] = req.queryStr;		// on URL after ? Ex: www.test.com/script.sh?a=10&b=20 , query string = a=10&b=20
	_env["PATH_INFO"] = req.pathInfo;			// sub-resource path that come after script name
	_env["PATH_TRANSLATED"] = req.serv.root + req.pathInfo;	// --
	// for Server
	_env["SERVER_NAME"] = req.serv.name;		// name of server.
	_env["SERVER_PORT"] = req.serv.port;		// Port of server Ex: 8080
	// Special
	_env["AUTH_TYPE"] = "";						// Use for identify user
	_env["REMOTE_ADDR"] = "";					// IP address of client that request Ex: 127.0.0.1
	_env["REMOTE_HOST"] = "";					// host name of client that request
	_env["REMOTE_IDENT"] = "";					// empty
	_env["REMOTE_USER"] = "";					// empty
	// HTTP protocal Env
	std::map<std::string, std::string>::const_iterator	it;
	for (it = req.headers.begin(); it != req.headers.end(); it++)
		_env[toProtoEnv(it->first)] = it->second; 
	return true;
}

bool	CgiHandler::_checkCgiScript(short int & status, parsedReq & req) {
	// Check file is exist
	if (access(req.pathSrc.c_str(), F_OK) != 0) {
		Logger::isLog(DEBUG) && Logger::log(RED, "[CGI] - No such file or directory");
		status = 404;
		return false;
	}
	// Check permission
	if (access(req.pathSrc.c_str(), X_OK) != 0) {
		Logger::isLog(DEBUG) && Logger::log(RED, "[CGI] - Permission denied");
		status = 403;
		return false;
	}
	if (req.method == "POST")
		_isPost = 1;
	else
		_isPost = 0;
	// Check CGI-script extension
	size_t	found = req.pathSrc.find_last_of(".");
	if (found != std::string::npos) {
		std::string	ext = req.pathSrc.substr(found + 1);
		if (ext == "py") {
			_cgiProgramPath = PYTHON_PATH;
			return true;
		}
	}
	status = 500;
	Logger::isLog(DEBUG) && Logger::log(RED, "[CGI] - Not support extension type");
	return false;
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

bool	CgiHandler::_gotoCgiDir(std::string & srcPath) {
	std::size_t	found = srcPath.find_last_of("/");
	if (found != std::string::npos) {
		std::string	path = srcPath.substr(0, found);
		_cgiFileName = srcPath.substr(found + 1);
		if (chdir(path.c_str()) == -1)
			return false;
	}
	else
		_cgiFileName = srcPath;
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

void CgiHandler::_childProcess(parsedReq & req) {
	if (_isPost) { // if post method will take input from std::in
		dup2(_pipeInFd[0], STDIN_FILENO);
		_closePipe(_pipeInFd[1]);
		_closePipe(_pipeInFd[0]);
	}
	if (!_gotoCgiDir(req.pathSrc))
		exit(errno);
	dup2(_pipeOutFd[1], STDOUT_FILENO);
	_closePipe(_pipeOutFd[0]);
	_closePipe(_pipeOutFd[1]);
	char *args[3];
	args[0] = strdup(_cgiProgramPath);
	args[1] = strdup(_cgiFileName);
	args[2] = NULL;
	char **env = aopEnv(_env);
	if (execve(args[0], args, env) == -1) {
		free2Dstr(env);
		delete[] args[0];
		delete[] args[1];
		exit(errno);
	}
}
