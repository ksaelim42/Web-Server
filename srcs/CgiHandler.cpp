#include "CgiHandler.hpp"

bool	CgiHandler::sendRequest(short int & status, parsedReq & req) {
	std::cout << BYEL << "Start handler CGI" << RESET << std::endl;
	if (_checkCgiScript(status, req) == 0)
		return false;
	if (_initEnv(req) == 0)
		return false;
	if (_createPipe(req) == 0)
		return (status = 500, false);
	_pid = fork();
	if (_pid == -1)
		return (_closePipe(), status = 500, false);
	if (_pid == 0) // Child Process
		_childProcess(req);
	else { // Parent Process
		if (_isPost) {
			_package = 1;
			std::cout << BYEL << "---Body Request, size : " << req.body.size() << RESET << std::endl;
			fcntl(_pipeInFd[1], F_SETFL, O_NONBLOCK); // TODO : for test
			close(_pipeInFd[0]);
			if (req.body.size()) {
				write(_pipeInFd[1], req.body.c_str(), req.body.size());
				req.bodySent += req.body.size();
				std::cout << YEL << "CGI pakage[" << _package++ << "] sent " << req.bodySent << " out of " << req.bodySize << RESET << std::endl;
			}
			if (req.bodySent >= req.bodySize) {
				std::cout << YEL << "Success for sent CGI pagekage" << RESET << std::endl;
				close(_pipeInFd[1]);
				return 0;
			}
			else
				return 1;
		}
	}
	status = 200;
	return 0;
}

bool	CgiHandler::sendBody(char * body, size_t & bufSize, parsedReq & req) {
	ssize_t	bytes;

	bytes = write(_pipeInFd[1], body, bufSize);
	req.bodySent += bufSize;
	std::cout << YEL << "CGI pakage[" << _package++ << "] sent " << req.bodySent << " out of " << req.bodySize << RESET << std::endl;
	if (req.bodySent >= req.bodySize) {
		std::cout << YEL << "Success for sent CGI pagekage" << RESET << std::endl;
		close(_pipeInFd[1]);
		return 0;
	}
	return 1;
}

bool	CgiHandler::receiveResponse(short int & status, std::string & cgiMsg) {
	int	WaitStat;
	waitpid(_pid, &WaitStat, 0);
	// std::cout << YEL << "wait status: " << WaitStat << strerror(WaitStat) << RESET << std::endl;
	// std::cout << YEL << "wait status: " << WIFEXITED(WaitStat) << RESET << std::endl;
	if (WaitStat != 0)
		return (status = 502, false);
	char	buffer[10000];
	size_t	bytesRead;
	memset(buffer, 0, 10000);
	bytesRead = read(_pipeOutFd[0], buffer, 10000);
	close(_pipeOutFd[1]);
	close(_pipeOutFd[0]);
	std::cout << "bytes read: " << bytesRead << std::endl;
	cgiMsg = buffer;
	// std::cout << YEL << cgiMsg << RESET << std::endl; // debug
	return (status = 200, true);
}

void CgiHandler::_childProcess(parsedReq & req) {
	if (_isPost) { // if post method will take input from std::in
		dup2(_pipeInFd[0], STDIN_FILENO);
		close(_pipeInFd[1]);
		close(_pipeInFd[0]);
	}
	_gotoCgiDir(req.pathSrc);
	dup2(_pipeOutFd[1], STDOUT_FILENO);
	close(_pipeOutFd[0]);
	close(_pipeOutFd[1]);
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

bool	CgiHandler::_initEnv(parsedReq & req) {
	_env.clear();
	// meta-variable
	_env["GATEWAY_INTERFACE"] = CGI_VERS;	// version of CGI
	_env["SERVER_SOFTWARE"] = PROGRAM_NAME;	// name of Webserv/version Ex: nginx/1.18.0
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
	// check file is exist
	if (access(req.pathSrc.c_str(), F_OK) != 0) {
		std::cerr << YEL << "No such file or directory" << RESET << std::endl;
		status = 404;
		return false;
	}
	// check permission
	if (access(req.pathSrc.c_str(), X_OK) != 0) {
		std::cerr << YEL << "Permission denied" << RESET << std::endl;
		status = 403;
		return false;
	}
	if (req.method == "POST")
		_isPost = 1;
	else
		_isPost = 0;
	size_t	found = req.pathSrc.find_last_of(".");
	if (found != std::string::npos) {
		std::string	ext = req.pathSrc.substr(found + 1);
		if (ext == "py") {
			_cgiProgramPath = PYTHON_PATH;
			return true;
		}
	}
	status = 500;
	std::cerr << YEL << "Program doesn't support to run this extension" << RESET << std::endl;
	return false;
}

bool	CgiHandler::_createPipe(parsedReq & req) {
	if (pipe(_pipeOutFd) == -1) // Sent output from Child to Parent
		return false;
	if (_isPost) {
		if (pipe(_pipeInFd) == -1) { // Sent input from Parent to Child
			close(_pipeOutFd[0]);
			close(_pipeOutFd[1]);
			return false;
		}
	}
	return true;
}

void	CgiHandler::_closePipe(void) {
	if (_isPost) {
		close(_pipeInFd[0]);
		close(_pipeInFd[1]);
	}
	close(_pipeOutFd[0]);
	close(_pipeOutFd[1]);
}

bool	CgiHandler::_gotoCgiDir(std::string & srcPath) {
	std::size_t	found = srcPath.find_last_of("/");
	if (found != std::string::npos) {
		std::string	path = srcPath.substr(0, found);
		_cgiFileName = srcPath.substr(found + 1);
		if (chdir(path.c_str()) == -1)
			exit(errno);
	}
	else
		_cgiFileName = srcPath;
	return true;
}