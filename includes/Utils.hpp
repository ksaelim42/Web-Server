#ifndef UTILS_HPP
# define UTILS_HPP

#include <cctype>
#include <string>
#include <map>
#include <vector>
#include <unistd.h>

#include "Logger.hpp"
class Server;
struct return_t;
struct Location;

// MAC
// typedef unsigned long long int uint64_t;

// Linux
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
short int	strToShortInt(std::string str);
// config utils
void	clearServer(Server &obj);
// void	clearLocation(Location &locStruct);
Location	clearLocation(Server const & server);
bool	scanPorts(std::vector<Server> servers);
// bool	scanPorts(std::string key, std::string value, std::vector<std::string> &ports);
void	printConfig(Server obj);
void	printServers(std::vector<Server> servers);

#endif
