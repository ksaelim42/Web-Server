// #include "../include/Webserv.hpp"
// #include "../include/HttpResponse.hpp"
#include "Socket.hpp"

int	main(int argc, char**argv)
{
	// std::cout << "Hello" << std::endl;
	(void)argc;
	(void)argv;
	try {
		Socket		socket;
		Server		server;

		// for (int i = 0; i < 1000; i++) {
		// 	int* myarray= new int[10000000000];
		// 	usleep(5000);
		// }
		socket.initServer(server);
		socket.runServer(server);
		socket.downServer(server);
		// HttpResponse	response(server, request);
		// response.createResponse();
		// response.prtResponse();
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	// catch (std::bad_alloc &ba) {
	// 	std::cerr << ba.what() << "There is not enough free memory to run Webserve" << std::endl;
	// 	return 1;
	// }
}