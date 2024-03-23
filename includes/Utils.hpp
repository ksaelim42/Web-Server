#ifndef UTILS_HPP
# define UTILS_HPP

# define RED			"\e[0;31m"
# define GRN			"\e[0;32m"
# define YEL			"\e[0;33m"
# define BLU			"\e[0;34m"
# define MAG			"\e[0;35m"
# define CYN			"\e[0;36m"
# define RESET			"\e[0m"
# define CRLF			"\r\n"

# define BBLU			"\e[1;34m"
# define BYEL			"\e[1;33m"

#include <cctype>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>

bool		isHexChar(char &);
bool		isHexStr(std::string &);
void		free2Dstr(char** &);
void		prtMap(std::map<std::string, std::string> &);
char*		strdup(std::string);
char**		aopArgs(std::vector<std::string> &);
char**		aopEnv(std::map<std::string, std::string> &);
size_t		hexStrToDec(std::string);
uint64_t	strToNum(std::string);
std::string	numToStr(size_t);
std::string	toProtoEnv(std::string);
std::string	toProperCase(std::string &);
std::string	strCutTo(std::string &, std::string );
std::string	findHeaderValue(std::map<std::string, std::string> &, std::string const &);

#endif
