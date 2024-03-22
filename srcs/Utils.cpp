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

size_t	hexStrToDec(std::string str) {
	unsigned int		num;
	std::istringstream	iss(str);
	iss >> std::hex >> num;
	return num;
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
