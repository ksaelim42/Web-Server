#ifndef UTILS_HPP
# define UTILS_HPP

#include <cctype>
#include <string>
#include <map>
#include <vector>
#include <unistd.h>

#include "Logger.hpp"

typedef unsigned long int uint64_t;

bool		isHexChar(char &);
bool		isHexStr(std::string &);
void		free2Dstr(char** &);
void		prtMap(std::map<std::string, std::string> &);
char*		strdup(std::string);
char**		aopArgs(std::vector<std::string> &);
char**		aopEnv(std::map<std::string, std::string> &);
size_t		hexStrToDec(std::string);
uint64_t	strToNum(std::string);
std::string	numToStr(ssize_t);
std::string	toProtoEnv(std::string);
std::string	toProperCase(std::string &);
std::string	strCutTo(std::string &, std::string );
std::string	findHeaderValue(std::map<std::string, std::string> &, std::string const &);

#endif
