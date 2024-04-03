/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsingConfig.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prachman <prachman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/13 11:14:49 by prachman          #+#    #+#             */
/*   Updated: 2024/04/03 12:48:19 by prachman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <map>

void storeDirectives(Server &obj, std::string key, std::string value, std::vector<std::string> valueVec)
{
	if (key == "server_name")
		obj.name = value;
	else if (key == "root")
		obj.root = value;
	else if (key == "listen")
	{
		obj.ipAddr = value;
		obj.port = value;
	}
	else if (key == "client_max_body_size")
	{
		uint64_t maxSize;

		maxSize = strToNum(value);
		if (maxSize > UINT64_MAX) //? mac printed out 'error', not sure if linux will do the same
			return;				  // ! more later
		obj.cliBodySize = maxSize;
	}
	else if (key == "autoindex")
	{
		obj.autoIndex = false;
		if (value == "on")
			obj.autoIndex = true;
	}
	else if (key == "cgi_pass")
	{
		obj.cgiPass = false;
		if (value == "on")
			obj.cgiPass = true;
	}
	else if (key == "limit_except")
	{
		obj.allowMethod = 0;
		for (std::vector<std::string>::iterator it = valueVec.begin(); it != valueVec.end(); it++)
		{
			if (*it == "GET")
				SET_METHOD(obj.allowMethod, METHOD_GET);
			else if (*it == "HEAD")
				SET_METHOD(obj.allowMethod, METHOD_HEAD);
			else if (*it == "POST")
				SET_METHOD(obj.allowMethod, METHOD_POST);
			else if (*it == "DELETE")
				SET_METHOD(obj.allowMethod, METHOD_DEL);
		}
	}
	else if (key == "return")
	{
		obj.retur.have = false;
		if (!value.empty())
		{
			short int status;

			obj.retur.have = true;
			status = strToShortInt(valueVec[0]);
			obj.retur.code = status;
			obj.retur.text = valueVec[1];
		}
	}
	else if (key == "index")
	{
		obj.index.clear(); // to make sure that the buffer won't messed up
		for (int i = 0; i < valueVec.size(); i++)
			obj.index.push_back(valueVec[i]);
	}
	else if (key == "error_page")
	{
		int valuePos = valueVec.size() - 1;
		short int errStatus;

		std::map<short int, std::string>::iterator it = obj.errPage.begin();
		for (int i = 0; i < valuePos; i++)
		{
			errStatus = strToShortInt(valueVec[i]);
			obj.errPage[errStatus] = valueVec[valuePos];
			// std::cout << it->first << " => " << it->second << std::endl;
			// it++;
		}
	}
}

void storeLocation(Location &locStruct, std::string key, std::string value, std::vector<std::string> valueVec)
{
	if (key == "location")
		locStruct.path = value;
	else if (key == "root")
		locStruct.root = value;
	else if (key == "client_max_body_size")
	{
		uint64_t maxSize;

		maxSize = strToNum(value);
		if (maxSize > UINT64_MAX) //? mac printed out 'error', not sure if linux will do the same
			return;				  // ! more later
		locStruct.cliBodySize = maxSize;
	}
	else if (key == "autoindex")
	{
		locStruct.autoIndex = false;
		if (value == "on")
			locStruct.autoIndex = true;
	}
	else if (key == "cgi_pass")
	{
		locStruct.cgiPass = false;
		if (value == "on")
			locStruct.cgiPass = true;
	}
	else if (key == "limit_except")
	{
		locStruct.allowMethod = 0;
		for (std::vector<std::string>::iterator it = valueVec.begin(); it != valueVec.end(); it++)
		{
			if (*it == "GET")
				SET_METHOD(locStruct.allowMethod, METHOD_GET);
			else if (*it == "HEAD")
				SET_METHOD(locStruct.allowMethod, METHOD_HEAD);
			else if (*it == "POST")
				SET_METHOD(locStruct.allowMethod, METHOD_POST);
			else if (*it == "DELETE")
				SET_METHOD(locStruct.allowMethod, METHOD_DEL);
		}
	}
	else if (key == "return")
	{
		locStruct.retur.have = false;
		if (!value.empty())
		{
			short int status;

			locStruct.retur.have = true;
			status = strToShortInt(valueVec[0]);
			locStruct.retur.code = status;
			locStruct.retur.text = valueVec[1];
		}
	}
	else if (key == "index")
	{
		for (int i = 0; i < valueVec.size(); i++)
			locStruct.index.push_back(valueVec[i]);
	}
}

void setValue(Server &obj, Location &locStruct, std::string key, std::string value, bool isLocation)
{
	std::string manyCase[] = {"index", "limit_except", "error_page", "return"};
	std::vector<std::string> valueVec;
	bool isMany = false;

	// std::cout << value << std::endl;
	for (int i = 0; i < 4; i++)
	{
		if (key == manyCase[i])
		{
			isMany = true;
			break;
		}
	}
	if (isMany)
	{
		for (int i = 0; i < value.length(); i++)
		{
			std::string tmp;
			while (value[i] != ' ' && i < value.length())
				tmp += value[i++];
			if (!tmp.empty())
				valueVec.push_back(tmp);
		}
		// for (std::vector<std::string>::iterator it = valueVec.begin(); it != valueVec.end(); it++)
		// 	std::cout << *it << std::endl;
		// exit(0);
	}
	if (isLocation)
		storeLocation(locStruct, key, value, valueVec);
	else
		storeDirectives(obj, key, value, valueVec);
}

int main(int ac, char **av)
{
	std::ifstream configFile;
	std::string tmp;
	std::map<std::string, std::string> mapConfig;
	Server obj;
	Location locStruct;
	bool isLocation = false;

	if (ac != 2)
		return (std::cout << "must have 2 arguments" << std::endl, 0);
	configFile.open(av[1]);
	if (!configFile.is_open())
		return (std::cout << "cannot open config file" << std::endl, 0);
	while (std::getline(configFile, tmp))
	{
		std::string key;
		int i = 0;
		// for (int i = 0; tmp[i]; i++)
		while (tmp[i])
		{
			if (tmp[i] == ' ')
				i++;
			else
			{
				// copy a directive
				while (!isspace(tmp[i]))
					key += tmp[i++];
				break;
			}
			i++;
		}
		std::string value;
		while (tmp[i] && key != "server")
		{
			while (isspace(tmp[i]))
				i++;
			// copy value(s)
			while (tmp[i])
			{
				// std::cout << tmp[i] << std::endl;
				value += tmp[i++];
				// if (tmp[i] == ';')
				// 	break ;
			}
		}
		// remove ; from the value
		for (int j = 0; j < value[j]; j++)
		{
			if (value[j] == ';')
				value.erase(value.begin() + j);
		}
		if (key == "location" || isLocation)
		{
			std::string locKey = "location";
			isLocation = true;
			if (key == "}")
			{
				isLocation = false;
				obj.location.push_back(locStruct);
			}
			if (isLocation)
			{
				setValue(obj, locStruct, locKey, value, 1);
				continue;
			}
		}
		if (key != "location")
			setValue(obj, locStruct, key, value, 0);
		// std::cout << key << " : ";
		// std::cout << value << std::endl;
		// break ;
	}
	printConfig(obj);
}

//! NOTE
// directives that are nor 'location' are stored just fine
// location is not yet stored