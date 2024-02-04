#include "../include/Cgi.hpp"

// bool	Cgi::CgiHandler() {
// char*	strdup(std::string src) {
// 	if (!str)
// 		return nullptr;
// 	char*	dest = new[src.length()];
// 	std::strcpy()
// }

// }

char*	strdup(std::string src) {
	char*	dest = new char[src.length() + 1];
	for (size_t i = 0; i <= src.length(); i++)
		dest[i] = src[i];
	return (dest);
}

char**	Cgi::createArgs(std::string path) {
	char**	args = new char*[2];

	args[0] = strdup(path);
	args[1] = NULL;
	return (args);
}

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

		char	**args = createArgs(path);

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