// #include "Utils.hpp"
#include "../includes/Utils.hpp"
#include "../includes/Server.hpp"

std::string	toProtoEnv(std::string var) {
	std::string	key = "HTTP_";
	for (int i = 0; i < var.length(); i++) {
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

std::string	numToStr(size_t num) {
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
std::string	findHeaderValue(std::map<std::string, std::string> & map, std::string const & content) {
	std::map<std::string, std::string>::const_iterator	it;
	std::string	value = "";

	it = map.find(content);
	if (it != map.end()) {
		value = it->second;
		map.erase(it);
	}
	return value;
}

void	prtMap(std::map<std::string, std::string> & set) {
	std::map<std::string, std::string>::const_iterator	it;
	size_t	i = 0;
	for (it = set.begin(); it != set.end(); it++)
		std::cout << it->first << ": " << it->second << std::endl;
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
	}
}







