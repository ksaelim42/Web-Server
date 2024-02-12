#include "CgiHandler.hpp"

CgiHandler::CgiHandler() {
	_env["AUTH_TYPE"] = "";					// Use for identify user
	_env["CONTENT_LENGTH"] = "";			// Must specify on POST method for read body content
	_env["CONTENT_TYPE"] = "test";			// get from request
	_env["GATEWAY_INTERFACE"] = CGI_VERS;	// version of CGI
	_env["PATH_INFO"] = "test";				// sub-resource path that come after script name
	_env["PATH_TRANSLATED"] = "test";		// --
	_env["QUERY_STRING"] = "test";			// on URL after ? Ex: www.test.com/script.sh?a=10&b=20 , query string = a=10&b=20
	_env["REMOTE_ADDR"] = "test";			// IP address of client that request
	_env["REMOTE_HOST"] = "test";			// --
	_env["REMOTE_IDENT"] = "test";			// --
	_env["REMOTE_USER"] = "test";			// empty
	_env["REQUEST_METHOD"] = "test";		// HTTP method Ex: GET
	_env["SCRIPT_NAME"] = "test";			// name of script (always have forward slash) Ex: /script.sh
	_env["SERVER_NAME"] = "test";			// empty
	_env["SERVER_PORT"] = "test";			// Port of server Ex: 8080
	_env["SERVER_PROTOCOL"] = "";			// HTTP version that get from request (Server must check that support) Ex : HTTP/1.1
	_env["SERVER_SOFTWARE"] = PROGRAM_NAME;	// name of webserver / version Ex: nginx/1.18.0
}

// bool	CgiHandler::CgiHandlerHandler() {
// char*	strdup(std::string src) {
// 	if (!str)
// 		return nullptr;
// 	char*	dest = new[src.length()];
// 	std::strcpy()
// }

// }

std::string	CgiHandler::execCgiScript(Server & server, request_t & request, short int & statusCode) {
	std::string	message;
	std::cout << "Start handler CGI" << std::endl;
	std::cout << request.path << std::endl;
	// Check path
	// Check extension file must be sh for mandatory
	// Check allow method ? still don't know why have to check
	// init env : set all env get input from request
	// pipe
	(void)statusCode;
	if (_createCgiRequest(server, request) == 0) {
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
		sleep(2); // Why
		write(_pipeInFd[1], request.body.c_str(), request.body.size());
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

bool	CgiHandler::_createCgiRequest(Server & server, request_t & request) {
	_path = request.path;
	// add env
	// _env["CONTENT_LENGTH"] = "";
	// _env["CONTENT_TYPE"] = "test";
	// _env["PATH_INFO"] = "test";
	// _env["PATH_TRANSLATED"] = "test";
	// _env["QUERY_STRING"] = "test";
	// _env["REMOTE_ADDR"] = "test";
	// _env["REMOTE_HOST"] = "test";
	// _env["REMOTE_IDENT"] = "test";
	// _env["REMOTE_USER"] = "test";
	// _env["REQUEST_METHOD"] = "test";
	// _env["SCRIPT_NAME"] = "test";
	// _env["SERVER_NAME"] = ;
	// _env["SERVER_PORT"] = "test";
	// _env["SERVER_PROTOCOL"] = "";

	// check path cgi is true
	// check permission
	// (void)request;
	return true;
}

