#include "CgiHandler.hpp"

CgiHandler::CgiHandler() {
	_env["GATEWAY_INTERFACE"] = CGI_VERS;	// version of CGI
	_env["SERVER_SOFTWARE"] = PROGRAM_NAME;	// name of Webserv/version Ex: nginx/1.18.0
}

short int	CgiHandler::execCgiScript(parsedReq & req, std::string & message) {
	std::cout << "Start handler CGI" << std::endl;
	if (_checkCgiScript(req.pathSrc) == 0)
		return _status;
	if (_initEnv(req) == 0)
		return _status;
	if (_createPipe() == 0)
		return 500;
	_pid = fork();
	if (_pid == -1)
		return 500;
	else if (_pid == 0) { // Child Process
		// close unnecessary pipe
		close(_pipeInFd[1]);
		close(_pipeOutFd[0]);
		// redirect stdard in and out
		dup2(_pipeInFd[0], STDIN_FILENO);
		dup2(_pipeOutFd[1], STDOUT_FILENO);
		close(_pipeInFd[0]);
		close(_pipeOutFd[1]);
		char	*args[2];
		args[0] = strdup(req.pathSrc);
		args[1] = NULL;
		char	**env = aopEnv(_env);
		if (execve(args[0], args, env) == -1)
			exit(errno);
	}
	else { // Parent Process
		// close unnecessary pipe
		close(_pipeInFd[0]);
		close(_pipeOutFd[1]);

		// Server Section
		write(_pipeInFd[1], req.body.c_str(), req.body.size());
		close(_pipeInFd[1]);

		// Read Output
		char	buffer[10000];
		size_t	bytesRead;
		size_t	i = 0;
		bytesRead = read(_pipeOutFd[0], &buffer[i], 10000);
		// while (i < HEADBUFSIZE) {
		// 	bytesRead = read(_pipeOutFd[0], &buffer[i], 1);
		// 	if (buffer[i] == '\n' && i && buffer[i - 1] == '\n') {
		// 		i
		// 	}
		// }
		close(_pipeOutFd[0]);
		std::cout << "bytes read: " << bytesRead << std::endl;
		std::cout << buffer;
		message = buffer;
	}
	return 200;
}

bool	CgiHandler::_createPipe(void) {
	if (pipe(_pipeInFd) == -1) { // Sent input from Parent to Child
		return false;
	}
	if (pipe(_pipeOutFd) == -1) { // Sent output from Child to Parent
		return false;
	}
	return true;
}

bool	CgiHandler::_checkCgiScript(std::string path) {
	// check file is exist
	if (access(path.c_str(), F_OK) != 0) {
		std::cerr << RED << "No such file or directory" << RESET << std::endl;
		// _status = ;
		return false;
	}
	// check permission
	if (access(path.c_str(), X_OK) != 0) {
		std::cerr << RED << "Permission denied" << RESET << std::endl;
		// _status = ;
		return false;
	}
	// Check extension file must be sh for mandatory
	// Check allow method ? still don't know why have to check
	return true;
}

bool	CgiHandler::_initEnv(parsedReq & req) {
	// meta-variable
	_env["REQUEST_METHOD"] = req.method;		// HTTP method Ex: GET
	_env["REQUEST_URI"] = req.uri;				// URI (not encode URL)
	_env["SERVER_PROTOCOL"] = req.version;		// HTTP version that get from request (Server must check that support) Ex : HTTP/1.1
	_env["CONTENT_LENGTH"] = req.contentLength;	// Must specify on POST method for read body content
	_env["CONTENT_TYPE"] = req.contentType;		// get from request
	// Parsing
	_env["SCRIPT_NAME"] = req.pathSrc;			// path of script (exclude Query string & path info) Ex: /script.sh
	_env["QUERY_STRING"] = req.queryStr;		// on URL after ? Ex: www.test.com/script.sh?a=10&b=20 , query string = a=10&b=20
	_env["PATH_INFO"] = req.pathInfo;			// sub-resource path that come after script name
	_env["PATH_TRANSLATED"] = req.serv.getRoot() + req.pathInfo;	// --
	// for Server
	_env["SERVER_NAME"] = req.serv.getName();	// name of server.
	_env["SERVER_PORT"] = req.serv.getPort();	// Port of server Ex: 8080
	// Special
	// _env["AUTH_TYPE"] = "";						// Use for identify user
	_env["REMOTE_ADDR"] = "";				// IP address of client that request Ex: 127.0.0.1
	_env["REMOTE_HOST"] = "";				// host name of client that request
	_env["REMOTE_IDENT"] = "";				// empty
	_env["REMOTE_USER"] = "";				// empty
	// HTTP protocal Env
	std::map<std::string, std::string>::const_iterator	it;
	for (it = req.headers.begin(); it != req.headers.end(); it++)
		_env[toProtoEnv(it->first)] = it->second; 
	return true;
}
