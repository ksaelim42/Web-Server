#include "CgiHandler.hpp"

CgiHandler::CgiHandler() {}

CgiHandler::~CgiHandler() {}

void	CgiHandler::_initCgi() {
	_pid = -1;
	_isPost = 0;
	_pipeInFd[0] = -1;
	_pipeInFd[1] = -1;
	_pipeOutFd[0] = -1;
	_pipeOutFd[1] = -1;
	_cgiFileName = "";
	_cgiProgramPath = "";
	_env.clear();
}

bool	CgiHandler::createRequest(Client & client) {
	Logger::isLog(DEBUG) && Logger::log(YEL, "[CGI] - Start");
	parsedReq&	req = client.getRequest();
	_initCgi();
	if (!_checkCgiScript(client.status, req))
		return false;
	if (!_initEnv(req))
		return false;
	if (!_createPipe()) {
		Logger::isLog(DEBUG) && Logger::log(RED, "[CGI] - Error for create Pipe");
		return client.status = 500, false;
	}
	if (!_setNonBlocking()) {
		Logger::isLog(DEBUG) && Logger::log(RED, "[CGI] - Error for setting Non-blocking I/O");
		return _closeAllPipe(), client.status = 500, false;
	}
	_pid = fork();
	if (_pid == -1) {
		Logger::isLog(DEBUG) && Logger::log(RED, "[CGI] - Error for fork child");
		return _closeAllPipe(), client.status = 500, false;
	}
	if (_pid == 0) // Child Process
		_childProcess(req);
	else { // Parent Process
		close(_pipeOutFd[1]);
		if (_isPost) {
			close(_pipeInFd[0]);
			client.addPipeFd(_pipeInFd[1], PIPE_IN);
			req.package = 0;
			if (req.bodyType == CHUNKED_ENCODE)
				req.type = CHUNK;
			else
				req.type = BODY;
		}
		else
			client.setResType(CGI_RES);
		Logger::isLog(DEBUG) && Logger::log(YEL, "[CGI] - fork child pid: ", _pid);
		client.pid = _pid;
		client.addPipeFd(_pipeOutFd[0], PIPE_OUT);
	}
	return true;
}

bool	CgiHandler::sendBody(Client & client, int fd) {
	ssize_t		bytes = 0;
	parsedReq&	req = client.getRequest();

	if (client.reqBody.length()) {
		bytes = write(fd, client.reqBody.c_str(), client.reqBody.length());
		if (bytes < 0) {
			client.status = 502;
			client.setResType(ERROR_RES);
			Logger::isLog(DEBUG) && Logger::log(RED, "[CGI] - Error writing");
			return false;
		}
		client.reqBody.erase(0, bytes);
		req.bodySent += bytes;
		req.package++;
	}
	if (req.type == CHUNK) {
		Logger::isLog(WARNING) && Logger::log(YEL, "[CGI] - chunk[", req.package, "] sent ", bytes, " Bytes");
	}
	else {
		Logger::isLog(WARNING) && Logger::log(YEL, "[CGI] - pakage[", req.package, "] sent ", req.bodySent, " out of ", req.bodySize);
		if (req.bodySent >= req.bodySize) {
			client.delPipeFd(fd, PIPE_IN);
			client.setResType(CGI_RES);
			Logger::isLog(DEBUG) && Logger::log(YEL, "[CGI] - Success for sent pagekage -----");
		}
	}
	return true;
}

ssize_t	CgiHandler::receiveResponse(Client & client, int fd, char* buffer) {
	int				WaitStat = 0;
	int				status = 0;
	ssize_t			bytes;
	HttpResponse&	res = client.getResponse();

	Logger::isLog(ERROR) && Logger::log(YEL, "[CGI] - Receive Response");
	status = waitpid(client.pid, &WaitStat, WNOHANG);
	Logger::isLog(ERROR) && Logger::log(YEL, "[CGI] - Pid Status: ", WaitStat);
	if (WaitStat != 0) {
		client.status = 502;
		client.delPipeFd(fd, PIPE_OUT);
		client.setResType(ERROR_RES);
		return -1;
	}
	if (status == 0)
		return 0;
	bytes = read(fd, buffer, LARGEFILESIZE);
	if (bytes == -1) {
		client.status = 502;
		client.delPipeFd(fd, PIPE_OUT);
		client.setResType(ERROR_RES);
		return false;
	}
	res.body.assign(buffer, bytes);
	res.bodySent += bytes;
	Logger::isLog(DEBUG) && Logger::log(YEL, "[CGI] - Receive Data form Cgi-script: ", bytes, " Bytes");
	if (res.bodySent >= res.bodySize) {
		client.pid = -1;
		client.delPipeFd(fd, PIPE_OUT);
	}
	return bytes;
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
	// HTTP protocol Env
	std::map<std::string, std::string>::const_iterator	it;
	for (it = req.headers.begin(); it != req.headers.end(); it++)
		_env[toProtoEnv(it->first)] = it->second; 
	return true;
}

bool	CgiHandler::_checkCgiScript(short int & status, parsedReq & req) {
	// Check file is exist
	if (access(req.pathSrc.c_str(), F_OK) != 0) {
		Logger::isLog(DEBUG) && Logger::log(RED, "[CGI] - No such file or directory");
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

void	CgiHandler::_closePipe(int (&pipeFd)[2]) {
	close(pipeFd[0]);
	close(pipeFd[1]);
	// pipeFd[0] = -1;
	// pipeFd[1] = -1;
}

void	CgiHandler::_closeAllPipe(void) {
	if (_isPost) {
		_closePipe(_pipeInFd);
	}
	_closePipe(_pipeOutFd);
}

void CgiHandler::_childProcess(parsedReq & req) {
	if (_isPost) { // if post method will take input from std::in
		dup2(_pipeInFd[0], STDIN_FILENO);
		_closePipe(_pipeInFd);
	}
	if (!_gotoCgiDir(req.pathSrc))
		exit(errno);
	dup2(_pipeOutFd[1], STDOUT_FILENO);
	_closePipe(_pipeOutFd);
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
