#include "WebServer.hpp"
#include "Struct.hpp"

bool	matchLocation(std::vector<Server> servers, std::string path) {
	return true;
}

Location	createLocation(std::string path, std::string root, \
std::vector<std::string> index, uint16_t method, bool autoIndex, \
size_t cliSize, return_t retur, bool cgiPass) {
	Location	loc;
	loc.path = path;
	loc.root = root;
	loc.index = index;
	loc.allowMethod = method;
	loc.autoIndex = autoIndex;
	loc.cliBodySize = cliSize;
	loc.retur = retur;
	loc.cgiPass = cgiPass;
	return loc;
}

Server	createServer(void) {
	Server	serv;
	serv.setName("testwebserv1");
	serv.setPort("8003");
	serv.setRoot("docs/myPage");
	std::vector<std::string>	index;
	return_t	retur;
	retur.have = 0;
	retur.code = 0;
	retur.text = "";
	return_t	retur1;
	retur1.have = 1;
	retur1.code = 301;
	retur1.text = "/simple.html";
	uint16_t	method = -1;
	serv.setLocation(createLocation("/", "docs/myPage", index, method, 0, BODYBUFSIZE, retur, 0)); // basic page
	serv.setLocation(createLocation("/redir", "docs/myPage", index, method, 0, BODYBUFSIZE, retur1, 0)); // redirection path
	serv.setLocation(createLocation("/cgi-bin", ".", index, method, 0, BODYBUFSIZE, retur, 1)); // test cgi path
	serv.setLocation(createLocation("/blog", "docs/myPage", index, method, 1, BODYBUFSIZE, retur, 0)); // test auto index on /blog path
	std::vector<std::string>	index1;
	index1.push_back("Cat03.jpg");
	index1.push_back("test.png");
	serv.setLocation(createLocation("/images", "docs/myPage", index1, method, 0, BODYBUFSIZE, retur, 0));
	return serv;
}

int	main(int argc, char**argv)
{
	// std::cout << "Hello" << std::endl;
	(void)argc;
	(void)argv;
	try {
		WebServer		webserv;
		std::vector<Server>		servs;

		servs.push_back(createServer());
		webserv.initServer(servs);
		servs[0].prtServer();
		webserv.runServer(servs);
		webserv.downServer(servs);
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