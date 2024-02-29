#ifndef UTILS_HPP
# define UTILS_HPP

# define RED			"\e[0;31m"
# define GREEN			"\e[0;32m"
# define YELLOW			"\e[0;33m"
# define PURPLE			"\e[0;35m"
# define RESET			"\e[0m"
# define CRLF			"\r\n"

#include <cctype>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>

std::string	numToStr(size_t);
std::string	toProtoEnv(std::string);
std::string	toProperCase(std::string &);
std::string	strCutTo(std::string &, std::string );
size_t		strToNum(std::string);
char*		strdup(std::string);
char**		aopArgs(std::vector<std::string> &);
char**		aopEnv(std::map<std::string, std::string> &);

void	prtMap(std::map<std::string, std::string> &);

#endif