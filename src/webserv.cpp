#include "webserv.hpp"

int	main(void) {
	Cgi	cgi;

	cgi.handlerCgi("cgi-bin/test.sh");
}