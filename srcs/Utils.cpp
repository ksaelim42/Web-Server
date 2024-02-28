#include "Utils.hpp"

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
	dest[0] = toupper(dest[0]);
	for (size_t i = 1; i < dest.length(); i++)
		dest[i] = tolower(dest[i]);
	return dest;
}

std::string	numToStr(size_t num) {
	std::stringstream	ss;
	ss << num;
	return ss.str();
}

size_t	strToNum(std::string str) {
	size_t				num;
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


void	prtMap(std::map<std::string, std::string> & set) {
	std::map<std::string, std::string>::const_iterator	it;
	size_t	i = 0;
	for (it = set.begin(); it != set.end(); it++)
		std::cout << it->first << ": " << it->second << std::endl;
}