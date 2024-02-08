#include "Webserv.hpp"

Cgi::Cgi() {
	_env["AUTH_TYPE"] = "test";
	_env["CONTENT_LENGTH"] = "test";
	_env["CONTENT_TYPE"] = "test";
	_env["GATEWAY_INTERFACE"] = "test";
	_env["PATH_INFO"] = "test";
	_env["PATH_TRANSLATED"] = "test";
	_env["QUERY_STRING"] = "test";
	_env["REMOTE_ADDR"] = "test";
	_env["REMOTE_HOST"] = "test";
	_env["REMOTE_IDENT"] = "test";
	_env["REMOTE_USER"] = "test";
	_env["REQUEST_METHOD"] = "test";
	_env["SCRIPT_NAME"] = "test";
	_env["SERVER_NAME"] = "test";
	_env["SERVER_PORT"] = "test";
	_env["SERVER_PROTOCOL"] = "test";
	_env["SERVER_SOFTWARE"] = "test";
}
// bool	Cgi::CgiHandler() {
// char*	strdup(std::string src) {
// 	if (!str)
// 		return nullptr;
// 	char*	dest = new[src.length()];
// 	std::strcpy()
// }

// }

int	Cgi::handlerCgi(std::string path) {

	t_pipe	mypipe;
	std::cout << "Start handler CGI" << std::endl;
	// Check path

	// Check extension file must be sh for mandatory

	// Check allow method ? still don't know why have to check

	// init env : set all env get input from request

	// pipe
	if (pipe(mypipe.fd) == -1) {
		std::cerr << "Error Pipe" << std::endl;
	}

	mypipe.pid = fork();
	if (mypipe.pid == -1) {
		std::cerr << "Error Fork" << std::endl;
	}
	if (mypipe.pid == 0) {
		// Child for execute CGI script
		// std::cout << "I'm Child" << std::endl;
		// std::cout << path << std::endl;
		dup2(mypipe.fd[1], STDOUT_FILENO);
		close(mypipe.fd[0]);
		close(mypipe.fd[1]);

		std::vector<std::string>	tmpArgs;
		tmpArgs[0] = path;
		char	**args = aopdup(tmpArgs);



		execve(args[0], args, NULL);
		exit(1);
	}
	else {
		close(mypipe.fd[1]);
		char	buffer[1024];
		std::string	body;

		int byteread = read(mypipe.fd[0], buffer, 1024);
		std::cout << "byte read: " << byteread << std::endl;
		// body = buffer;
		close(mypipe.fd[0]);
		std::cout << "I'm Parent" << std::endl;
		std::cout << buffer;
		// Parent for reading content form pipe

	}
		return 0;
}