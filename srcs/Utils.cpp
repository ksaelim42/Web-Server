// #include "Utils.hpp"
#include "../includes/Utils.hpp"
#include "../includes/Server.hpp"

std::string	toProtoEnv(std::string var) {
	std::string	key = "HTTP_";
	for (size_t i = 0; i < var.length(); i++) {
		if (var[i] == '-')
			var[i] = '_';
		var[i] = std::toupper(var[i]);
	}
	return key + var;
}

std::string	toProperCase(std::string & src) {
	if (src.empty())
		return "";
	std::string	dest(src);
	bool	first = 1;
	for (size_t i = 0; i < dest.length(); i++) {
		if (std::isalnum(dest[i]) || dest[i] == '_') {
			if (first) {
				dest[i] = std::toupper(dest[i]);
				first = 0;
			}
			else
				dest[i] = std::tolower(dest[i]);
		}
		else
			first = 1;
	}
	return dest;
}

std::string	numToStr(ssize_t num) {
	std::stringstream	ss;
	ss << num;
	return ss.str();
}

uint64_t	strToNum(std::string str) {
	uint64_t			num;
	std::stringstream	ss(str);
	ss >> num;
	return num;
}

size_t	hexStrToDec(std::string str) {
	unsigned int		num;
	std::istringstream	iss(str);
	iss >> std::hex >> num;
	return num;
}

bool	isHexChar(char & c) {
	if (((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'F' && c <= 'F')))
		return true;
	return false;
}

bool	isHexStr(std::string & str) {
	
	for (size_t i = 0;i < str.length(); i++) {
		if (!((str[i] >= '0' && str[i] <= '9') ||
			(str[i] >= 'a' && str[i] <= 'f') ||
			(str[i] >= 'F' && str[i] <= 'F')))
			return false;
	}
	return true;
}

short int	strToShortInt(std::string str) {
	short int			num;
	std::stringstream	ss(str);
	ss >> num;
	return num;
}

char*	strdup(std::string src) {
	char*	dest = new char[src.length() + 1];
	size_t	i = 0;
	for (; i < src.length(); i++)
		dest[i] = src[i];
	dest[i] = '\0';
	return (dest);
}

char**	aopEnv(std::map<std::string, std::string> & set) {
	char**	aop = new char*[set.size() + 1];
	std::map<std::string, std::string>::const_iterator	it = set.begin();
	size_t	i = 0;
	for (;it != set.end(); i++, it++) {
		aop[i] = strdup(it->first + "=" + it->second);
	}
	aop[i] = NULL;
	return aop;
}

void	free2Dstr(char** & ptr) {
	for (int i = 0; ptr[i]; i++)
		delete ptr[i];
	delete[] ptr;
	ptr = NULL;
}

char**	aopArgs(std::vector<std::string> & set) {
	if (set.empty())
		return NULL;
	char**	aop = new char*[set.size() + 1];
	size_t	i = 0;
	for (;i < set.size(); i++)
		aop[i] = strdup(set[i]);
	aop[i] = NULL;
	return aop;
}

// Find lim in string if found return word that end before lim and cut string include lim out
// if not found duplicate string to word and delete string
std::string	strCutTo(std::string & str, std::string lim) {
	std::size_t	found = str.find(lim);
	std::string word;
	if (found != std::string::npos) {
		word = str.substr(0, found);
		str.erase(0, found + lim.length());
	}
	else {
		word = str;
		str.clear();
	}
	return word;
}

// Finding special header in map headers
std::string	findHeaderValue(std::map<std::string, std::string> & myMap, std::string const & content) {
	std::map<std::string, std::string>::iterator	it;
	std::string	value = "";

	it = myMap.find(content);
	if (it != myMap.end()) {
		value = it->second;
		myMap.erase(it);
	}
	return value;
}

void	prtMap(std::map<std::string, std::string> & set) {
	std::map<std::string, std::string>::const_iterator	it;
	for (it = set.begin(); it != set.end(); it++)
		std::cout << it->first << ": " << it->second << std::endl;
}

// ************************************************************************** //
// ------------------------------ Config Utils ------------------------------ //
// ************************************************************************** //

void	clearServer(Server &obj)
{
	obj.name.clear();
	obj.port.clear();
	obj.ipAddr.clear();
	obj.root.clear();
	obj.autoIndex = false;
	obj.cgiPass = false;
	obj.allowMethod = 0;
	obj.cliBodySize = 0;
	obj.retur.have = 0;
	obj.retur.code = 0;
	obj.retur.text.clear();
	obj.location.clear();
	obj.errPage.clear();
	obj.index.clear();
}

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

bool	scanPorts(std::vector<Server> servers)
{
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		std::vector<Server>::iterator end = servers.end();
		for (std::vector<Server>::iterator start = it + 1; start != end; start++)
		{
			if (it->port == start->port)
				return (std::cout << "Error: Servers must not have the same ports" << std::endl, false);
		}
	}
	return true;
}

void	printServers(std::vector<Server> servers)
{
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		std::cout << "server_name: " << it->name << std::endl;
		std::cout << "clien_body_size: " << it->cliBodySize << std::endl;
		std::cout << "port: " << it->port << std::endl;
		std::cout << "ip: " << it->ipAddr << std::endl;
		std::cout << "root: " << it->root << std::endl;
		std::cout << "autoindex: " << it->autoIndex << std::endl;
		std::cout << "cgi: " << it->cgiPass << std::endl;
		std::cout << "method: " << it->allowMethod << std::endl;
		// return
		std::cout << "return:" << std::endl;
		std::cout << it->retur.have << std::endl;
		std::cout << it->retur.code << std::endl;
		std::cout << it->retur.text << std::endl;
		// index
		for (std::vector<std::string>::iterator index = it->index.begin(); index != it->index.end(); index++)
			std::cout << *index << std::endl;
		// error page
		for (std::map<short int, std::string>::iterator errPage = it->errPage.begin(); errPage != it->errPage.end(); errPage++)
			std::cout << errPage->first << " => " << errPage->second << std::endl;
		
		// locations
		if (it->location.size() < 1)
			continue ;
		int	locSize = it->location.size();
		for (int i = 0; i < locSize; i++)
		{
			std::cout << "#########LOCATION######### NO: " << i << std::endl;
			std::cout << "path: " << it->location[i].path << std::endl;
			std::cout << "root: " << it->location[i].root << std::endl;
			std::cout << "allowMethod: " << it->location[i].allowMethod << std::endl;
			std::cout << "autoIndex: " << it->location[i].autoIndex << std::endl;
			std::cout << "cliBodySize: " << it->location[i].cliBodySize << std::endl;
			std::cout << "cgiPass: " << it->location[i].cgiPass << std::endl;
			// return
			std::cout << "return :" << std::endl;
			std::cout << it->location[i].retur.have << std::endl;
			std::cout << it->location[i].retur.code << std::endl;
			std::cout << it->location[i].retur.text << std::endl;
			// index
			std::cout << "INDEX :" << std::endl;
			for (std::vector<std::string>::iterator index = it->location[i].index.begin(); index != it->location[i].index.end(); index++)
				std::cout << *index << std::endl;
			// std::cout << "end of location";
			std::cout << std::endl;
		}
		// std::cout << "Done looping all locations" << std::endl;
	}
	// std::cout << "Finished" << std::endl;
}

void	printConfig(Server obj)
{
	std::cout << "server_name: " << obj.name << "|" << std::endl;
	std::cout << "clien_body_size: " << obj.cliBodySize << std::endl;
	std::cout << "port: " << obj.port << "|" << std::endl;
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