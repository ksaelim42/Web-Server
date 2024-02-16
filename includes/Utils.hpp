#ifndef UTILS_HPP
# define UTILS_HPP

# define RED			"\e[0;31m"
# define GREEN			"\e[0;32m"
# define PURPLE			"\e[0;35m"
# define RESET			"\e[0m"
# define CRLF			"\r\n"

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>

std::string	numToStr(size_t);
std::string	toProtoEnv(std::string var);
size_t		strToNum(std::string);
std::map<std::string, std::string>	initMineTypeDefault(void);
char*	strdup(std::string);
char**	aopArgs(std::vector<std::string>);
char**	aopEnv(std::map<std::string, std::string>);

void	prtMap(std::map<std::string, std::string> set);

#endif