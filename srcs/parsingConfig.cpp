/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsingConfig.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prachman <prachman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/13 11:14:49 by prachman          #+#    #+#             */
/*   Updated: 2024/04/08 16:48:59 by prachman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "parsingConfig.hpp"
#include "../includes/parsingConfig.hpp"

void storeDirectives(Server &obj, std::string key, std::string value, std::vector<std::string> valueVec)
{
	if (key == "server_name")
		obj.name = value;
	else if (key == "root")
		obj.root = value;
	else if (key == "listen")
	{
		obj.ipAddr = "0.0.0.0";
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
		for (size_t i = 0; i < valueVec.size(); i++)
			obj.index.push_back(valueVec[i]);
	}
	else if (key == "error_page")
	{
		short int	errStatus;
		size_t		valuePos = valueVec.size() - 1;

		std::map<short int, std::string>::iterator it = obj.errPage.begin();
		if (!obj.errPage.empty()) // in case of incorrect buffer
			obj.errPage.erase(it, obj.errPage.end());
		for (size_t i = 0; i < valuePos; i++)
		{
			errStatus = strToShortInt(valueVec[i]);
			obj.errPage[errStatus] = valueVec[valuePos];
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
		{
			locStruct.autoIndex = true;
		}
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
		locStruct.index.clear(); // to make sure that the buffer won't messed up
		for (size_t i = 0; i < valueVec.size(); i++)
			locStruct.index.push_back(valueVec[i]);
	}
}

void setValue(Server &obj, Location &locStruct, std::string key, std::string value, bool isLocation)
{
	std::string manyCase[] = {"index", "limit_except", "error_page", "return"};
	std::vector<std::string> valueVec;
	bool isMany = false;

	for (size_t i = 0; i < 4; i++)
	{
		if (key == manyCase[i])
		{
			isMany = true;
			break;
		}
	}
	if (isMany)
	{
		for (size_t i = 0; i < value.length(); i++)
		{
			std::string tmp;
			while (!isspace(value[i]) && i < value.length())
				tmp += value[i++];
			if (!tmp.empty())
				valueVec.push_back(tmp);
		}
	}
	if (isLocation)
		storeLocation(locStruct, key, value, valueVec);
	else
		storeDirectives(obj, key, value, valueVec);
}

bool getLocation(Server &obj, Location &locStruct, std::string key, std::string value, bool &isLocation)
{
	if (key != "location" && !isLocation)
		return false;
	// remove { from the path
	for (size_t i = 0; i < value.length(); i++)
	{
		if (value[i] == '{')
			value.erase(value.begin() + i);
	}
	isLocation = true;
	if (key[0] == '}') // when read the line with } will result in key with length of 2. Therefore, use char as a condition
	{
		isLocation = false;
		obj.location.push_back(locStruct);
		clearLocation(locStruct);
		return false;
	}
	setValue(obj, locStruct, key, value, 1);
	return true;
}

std::string getValue(std::string value, std::string key, std::string line, int &i)
{
	while (line[i] && key != "server")
	{
		while (isspace(line[i]))
			i++;
		// copy value(s)
		while (line[i])
			value += line[i++];
	}
	// remove ; from the value
	for (int j = 0; j < value[j]; j++)
	{
		if (value[j] == ';')
			value.erase(value.begin() + j);
		if (value[j] == '\r')
			value.erase(value.begin() + j);
	}
	return value;
}

std::string	getKey(std::string key, std::string line, int &i)
{
	while (line[i])
	{
		if (!isspace(line[i]))
		{
			while (!isspace(line[i]))
				key += line[i++];
			break;
		}
		i++;
	}
	return key;
}

// int main(int ac, char **av)
// {
// 	std::ifstream configFile;
// 	std::string tmp;
// 	std::map<std::string, std::string> mapConfig;
// 	Server obj;
// 	Location locStruct;
// 	bool isLocation = false;

// 	if (ac != 2)
// 		return (std::cout << "must have 2 arguments" << std::endl, 0);
// 	configFile.open(av[1]);
// 	if (!configFile.is_open())
// 		return (std::cout << "cannot open config file" << std::endl, 0);
// 	while (std::getline(configFile, tmp))
// 	{
// 		int i = 0;
// 		std::string key;
// 		std::string value;

// 		key = getKey(key, tmp, i);
// 		value = getValue(value, key, tmp, i);
// 		if (getLocation(obj, locStruct, key, value, isLocation))
// 			continue;
// 		setValue(obj, locStruct, key, value, 0);
// 	}
// 	printConfig(obj);
// }
