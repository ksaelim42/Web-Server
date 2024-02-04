#include "../include/server.hpp"

int	main(int argc, char**argv)
{
	try {
		int	serverSock;
		Server	server;

		serverSock = server.initServer();
		server.runServer(serverSock);
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
}