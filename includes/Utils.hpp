#ifndef UTILS_HPP
# define UTILS_HPP

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>

std::string	numToStr(size_t);
size_t		strToNum(std::string);
std::map<std::string, std::string>	initMineTypeDefault(void);
char*	strdup(std::string);
char**	aopArgs(std::vector<std::string>);
char**	aopEnv(std::map<std::string, std::string>);

#endif