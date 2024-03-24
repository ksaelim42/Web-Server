#include "WebServer.hpp"

Location	createLocation(std::string path, std::string root, \
std::vector<std::string> index, uint16_t method, bool autoIndex, \
uint64_t cliSize, return_t retur, bool cgiPass) {
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
	serv.name = "localhost";
	serv.port = "8014";
	serv.root = "docs/myPage";
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
	serv.location.push_back(createLocation("/", "docs/myPage", index, method, 0, BODYBUFSIZE, retur, 0)); // basic page
	serv.location.push_back(createLocation("/redir", "docs/myPage", index, method, 0, BODYBUFSIZE, retur1, 0)); // redirection path
	serv.location.push_back(createLocation("/cgi-bin", "docs/myPage", index, method, 0, 5000000000, retur, 1)); // test cgi path
	serv.location.push_back(createLocation("/blog", "docs/myPage", index, method, 1, BODYBUFSIZE, retur, 0)); // test auto index on /blog path
	std::vector<std::string>	index1;
	index1.push_back("Cat03.jpg");
	index1.push_back("test.png");
	serv.location.push_back(createLocation("/images", "docs/myPage", index1, method, 0, BODYBUFSIZE, retur, 0));
	return serv;
}

Server	createServer2(void) {
	Server	serv;
	serv.name = "localhost";
	serv.port = "8020";
	serv.root = "docs/default";
	std::vector<std::string>	index;
	return_t	retur;
	retur.have = 0;
	retur.code = 0;
	retur.text = "";
	uint16_t	method = -1;
	serv.location.push_back(createLocation("/", "docs/default", index, method, 0, BODYBUFSIZE, retur, 0)); // basic page
	return serv;
}

int	main(int argc, char**argv)
{
	(void)argc; // TODO : set take config argument
	(void)argv;
	Logger::setLevel(WARNING);
	try {
		WebServer	webserv;
		std::vector<Server>		servs;
		servs.push_back(createServer());
		servs.push_back(createServer2());
		webserv.initServer(servs);

		webserv.runServer();
		webserv.downServer();
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}