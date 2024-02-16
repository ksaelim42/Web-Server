#include "Socket.hpp"

bool	matchLocation(std::vector<Server> servers, std::string path) {
	return true;
}

Server	createServer(void) {
	Server	serv;
	serv.setName("testwebserv1");
	serv.setPort("8000");
	serv.setRoot("html/static");
	return serv;
}

int	main(int argc, char**argv)
{
	// std::cout << "Hello" << std::endl;
	(void)argc;
	(void)argv;
	try {
		Socket		socket;
		std::vector<Server>		servs;

		servs.push_back(createServer());
		servs[0].prtServer();
		socket.initServer(servs);
		socket.runServer(servs);
		socket.downServer(servs);
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