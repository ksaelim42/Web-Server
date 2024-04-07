#ifndef PARSINGCONFIG_HPP
# define PARSINGCONFIG_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <stdint.h>

#include "Server.hpp"

void		setValue(Server &obj, Location &locStruct, std::string key, std::string value, bool isLocation);
bool		getLocation(Server &obj, Location &locStruct, std::string key, std::string value, bool &isLocation);
std::string	getValue(std::string value, std::string key, std::string line, int &i);
std::string	getKey(std::string key, std::string line, int &i);

#endif
