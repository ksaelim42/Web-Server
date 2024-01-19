#include "cgi.hpp"

// bool	Cgi::CgiHandler() {

// }

int	handlerCgi(std::string path) {

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
	// Child for execute CGI script
	if (mypipe.pid == 0) {

	}
	else
		return 0;
}