#ifndef UTILS_HPP
# define UTILS_HPP

#include <string>
#include <map>
#include <vector>
#include <sstream>

std::string	itoa(size_t num);
std::map<std::string, std::string>	initMineTypeDefault(void);
char*	strdup(std::string);
char**	aopdup(std::vector<std::string>);
char**	aopdup(std::map<std::string, std::string>);

#endif