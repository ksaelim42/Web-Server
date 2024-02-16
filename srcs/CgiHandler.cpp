#include "CgiHandler.hpp"

CgiHandler::CgiHandler() {
	_env["GATEWAY_INTERFACE"] = CGI_VERS;	// version of CGI
	_env["SERVER_SOFTWARE"] = PROGRAM_NAME;	// name of Webserv/version Ex: nginx/1.18.0
}

// bool	CgiHandler::CgiHandlerHandler() {
// char*	strdup(std::string src) {
// 	if (!str)
// 		return nullptr;
// 	char*	dest = new[src.length()];
// 	std::strcpy()
// }

// }

std::string	CgiHandler::execCgiScript(parsedReq & req, short int & statusCode) {
	std::string	message;
	std::cout << "Start handler CGI" << std::endl;
	std::cout << req.path << std::endl;
	// Check path
	// Check extension file must be sh for mandatory
	// Check allow method ? still don't know why have to check
	// init env : set all env get input from request
	// pipe
	(void)statusCode;
	if (_createCgiRequest(req) == 0) {
	// 	// TODO : set error
		// return false;
	}
	// if (_prepareScript() == 0) {
	// 	// TODO : set error
	// 	return false;
	// }
	if (_createPipe() == 0) {
		// TODO : set error = 500
		// return false;
	}
	_pid = fork();
	if (_pid == -1) {
		// TODO : set error = 500
		// return false;
	}
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
		args[0] = strdup(_path);
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
		// sleep(2); // Why
		write(_pipeInFd[1], req.body.c_str(), req.body.size());
		close(_pipeInFd[1]);
		char	buffer[5000];
		int bytesRead = read(_pipeOutFd[0], buffer, 5000);
		close(_pipeOutFd[0]);
		std::cout << "bytes read: " << bytesRead << std::endl;
		std::cout << buffer;
		message = buffer;
	}
	return message; // TODO
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

bool	CgiHandler::_prepareScript(void) {
	// check path cgi is true
	// check permission
	return true;
}

bool	CgiHandler::_createCgiRequest(parsedReq & req) {
	_path = req.path;
	// meta-variable
	_env["REQUEST_METHOD"] = req.method;	// HTTP method Ex: GET
	_env["REQUEST_URI"] = req.uri;			// URI (not encode URL)
	_env["SERVER_PROTOCOL"] = req.version;	// HTTP version that get from request (Server must check that support) Ex : HTTP/1.1
	_env["CONTENT_LENGTH"] = req.contentLength;	// Must specify on POST method for read body content
	_env["CONTENT_TYPE"] = req.contentType;		// get from request
	// Parsing
	_env["SCRIPT_NAME"] = req.path;			// path of script (exclude Query string & path info) Ex: /script.sh
	_env["QUERY_STRING"] = req.queryStr;	// on URL after ? Ex: www.test.com/script.sh?a=10&b=20 , query string = a=10&b=20
	_env["PATH_INFO"] = req.pathInfo;		// sub-resource path that come after script name
	// _env["PATH_TRANSLATED"] = "";				// --
	// for Server
	_env["SERVER_NAME"] = req.serv.getName();	// name of server.
	_env["SERVER_PORT"] = req.serv.getPort();	// Port of server Ex: 8080
	// Special
	// _env["AUTH_TYPE"] = "";					// Use for identify user
	_env["REMOTE_ADDR"] = "";			// IP address of client that request Ex: 127.0.0.1
	_env["REMOTE_HOST"] = "";				// host name of client that request
	_env["REMOTE_IDENT"] = "";				// empty
	_env["REMOTE_USER"] = "";				// empty
	// HTTP protocal Env
	std::map<std::string, std::string>::const_iterator	it;
	for (it = req.headers.begin(); it != req.headers.end(); it++)
		_env[toProtoEnv(it->first)] = it->second; 
	// check path cgi is true
	// check permission
	// (void)request;
	return true;
}

