// #include "../include/Webserv.hpp"
// #include "../include/HttpResponse.hpp"
#include "../include/Socket.hpp"

int	main(int argc, char**argv)
{
	// std::cout << "Hello" << std::endl;
	try {
		Socket		socket;
		Server		server;

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
}