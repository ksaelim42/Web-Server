#include "../include/Webserv.hpp"
#include "../include/HttpResponse.hpp"

void	genRequest(request_t & request) {
	// request.path = "./content/static/index.html";
	// request.path = "./content/static/style.css";
	// request.path = "./content/static/Cat.jpg";
	request.path = "./content/static/nothing";
}

int	main(int argc, char**argv)
{
	try {
		int	serverSock;
		Server	server;
		request_t	request;

		// serverSock = server.initServer();
		// server.runServer(serverSock);
		genRequest(request);
		HttpResponse	response(server, request);
		response.createResponse();
		response.prtResponse();
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
}