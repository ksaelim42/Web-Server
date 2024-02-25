#include "Socket.hpp"
#include "Struct.hpp"

bool	matchLocation(std::vector<Server> servers, std::string path) {
	return true;
}

Location	createLocation(std::string path, \
std::string root, std::vector<std::string> index, return_t retur) {
	Location	loc;
	loc.path = path;
	loc.root = root;
	loc.index = index;
	loc.retur = retur;
	return loc;
}

Server	createServer(void) {
	Server	serv;
	serv.setName("testwebserv1");
	serv.setPort("8003");
	serv.setRoot("html/default");
	std::vector<std::string>	index;
	std::vector<std::string>	index1;
	index1.push_back("Cat03.jpg");
	index1.push_back("test.png");
	// std::vector<std::string>	index2;
	return_t	retur;
	retur.have = 0;
	serv.setLocation(createLocation("/", "html/static", index, retur));
	serv.setLocation(createLocation("/images", "html/static", index1, retur));
	serv.setLocation(createLocation("/cgi-bin", ".", index, retur));
	serv.setLocation(createLocation("/redir", "html/static", index, return_t{1, 301, "/simple.html"}));
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
		socket.initServer(servs);
		servs[0].prtServer();
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