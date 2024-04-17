#include "WebServer.hpp"
#include "parsingConfig.hpp"

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

void	setErrPages(std::string root, std::map<short int, std::string> & errPage) {
	errPage[400] = root + "/" + "page/40x.html";
	errPage[403] = root + "/" + "page/40x.html";
	errPage[404] = root + "/" + "page/40x.html";
	errPage[405] = root + "/" + "page/40x.html";
}

Server	createServer(void) {
	Server	serv;
	serv.name = "localhost";
	serv.port = "8004";
	serv.root = "docs/myPage";
	setErrPages(serv.root, serv.errPage);
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

bool parsingConfig(int ac, char **av, std::vector<Server> & servers)
{
	std::ifstream configFile;
	std::string tmp;
	Server obj;
	Location locStruct;
	bool isLocation = false;
	bool firstServer = true;

	if (ac != 2)
		return (std::cout << "must have 2 arguments" << std::endl, 0);
	configFile.open(av[1]);
	if (!configFile.is_open())
		return (std::cout << "cannot open config file" << std::endl, 0);
	while (std::getline(configFile, tmp))
	{
		int i = 0;
		int	isSetLocation = 0;
		std::string key;
		std::string value;

		key = getKey(key, tmp, i);
		if (key == "server" && firstServer)
		{
			firstServer = false;
			continue;
		}
		value = getValue(value, key, tmp, i);
		isSetLocation = getLocation(obj, locStruct, key, value, isLocation);
		if (isSetLocation == 1)
			continue;
		else if (isSetLocation == 2)
			return 0;
		if (!setValue(obj, locStruct, key, value, 0))
			return 0;
		if (key == "server" || configFile.peek() == EOF)
		{
			servers.push_back(obj);
			clearServer(obj);
		}
	}
	if (!scanPorts(servers))
		return 0;
	printServers(servers);
	return true;
}

int	main(int argc, char** argv)
{
	std::vector<Server>		servs;

	if (!parsingConfig(argc, argv, servs))
		return 1;
	std::cout << "size of server: " << servs.size() << std::endl;
	prtMap(servs[0].errPage);
	// std::cout << "MimeType: " << servs[0].getMimeType("html") << std::endl;
	// exit(0);
	// servs[0].prtServer();
	// (void)argc;
	// (void)argv;
	// servs.push_back(createServer()); // TODO : gen by hand
	// servs.push_back(createServer2()); // TODO : gen by hand
	try {
		Logger::setLevel(DEBUG);
		WebServer	webserv;

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
