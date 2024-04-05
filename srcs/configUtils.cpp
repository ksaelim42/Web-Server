#include "../includes/Server.hpp"

void	clearLocation(Location &locStruct)
{
	locStruct.path.clear();
	locStruct.root.clear();
	locStruct.index.clear();
	locStruct.allowMethod = 0;
	locStruct.autoIndex = false;
	locStruct.cliBodySize = 0;
	locStruct.retur.have = 0;
	locStruct.retur.code = 0;
	locStruct.retur.text.clear();
}

void	printConfig(Server obj)
{
	std::cout << "server_name: " << obj.name << std::endl;
	std::cout << "clien_body_size: " << obj.cliBodySize << std::endl;
	std::cout << "port: " << obj.port << std::endl;
	std::cout << "ip: " << obj.ipAddr << std::endl;
	std::cout << "root: " << obj.root << std::endl;
	std::cout << "autoindex: " << obj.autoIndex << std::endl;
	std::cout << "cgi: " << obj.cgiPass << std::endl;
	std::cout << "method: " << obj.allowMethod << std::endl;
	// return
	std::cout << "return:" << std::endl;
	std::cout << obj.retur.have << std::endl;
	std::cout << obj.retur.code << std::endl;
	std::cout << obj.retur.text << std::endl;
	// index
	std::cout << "INDEX: " << std::endl;
	for (std::vector<std::string>::iterator it = obj.index.begin(); it != obj.index.end(); it++)
		std::cout << *it << std::endl;
	// error page
	std::cout << "err_page: " << std::endl;
	for (std::map<short int, std::string>::iterator it = obj.errPage.begin(); it != obj.errPage.end(); it++)
		std::cout << it->first << " => " << it->second << std::endl;
	
	// location 
	if (obj.location.size() < 1)
		return ;
	int	locSize = obj.location.size();
	for (int i = 0; i < locSize; i++)
	{
		std::cout << "#########LOCATION######### NO: " << i << std::endl;
		std::cout << "path: " << obj.location[i].path << std::endl;
		std::cout << "root: " << obj.location[i].root << std::endl;
		std::cout << "allowMethod: " << obj.location[i].allowMethod << std::endl;
		std::cout << "autoIndex: " << obj.location[i].autoIndex << std::endl;
		std::cout << "cliBodySize: " << obj.location[i].cliBodySize << std::endl;
		std::cout << "cgiPass: " << obj.location[i].cgiPass << std::endl;
		// return
		std::cout << "return :" << std::endl;
		std::cout << obj.location[i].retur.have << std::endl;
		std::cout << obj.location[i].retur.code << std::endl;
		std::cout << obj.location[i].retur.text << std::endl;
		// index
		std::cout << "INDEX :" << std::endl;
		for (std::vector<std::string>::iterator it = obj.location[i].index.begin(); it != obj.location[i].index.end(); it++)
			std::cout << *it << std::endl;
		std::cout << std::endl;
	}
}






