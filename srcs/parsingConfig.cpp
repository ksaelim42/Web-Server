// #include "parsingConfig.hpp"
#include "../includes/parsingConfig.hpp"

bool storeDirectives(Server &obj, std::string key, std::string value, std::vector<std::string> valueVec)
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
			return false;
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
			short int status = 0;

			obj.retur.have = true;
			status = strToShortInt(valueVec[0]);
			if (status < 100 || status > 599)
			{
				std::cout << "Return status must not be less than 1xx or more than 5xx" << std::endl;
				return false;
			}
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

		// std::map<short int, std::string>::iterator it = obj.errPage.begin();
		// if (!obj.errPage.empty()) // in case of incorrect buffer
		// 	obj.errPage.erase(it, obj.errPage.end());
		for (size_t i = 0; i < valuePos; i++)
		{
			errStatus = strToShortInt(valueVec[i]);
			obj.errPage[errStatus] = obj.root + "/" + valueVec[valuePos];
		}
	}
	return true;
}

bool storeLocation(Location &locStruct, std::string key, std::string value, std::vector<std::string> valueVec)
{
	if (key == "location") {
		size_t	num = 0;
		for (size_t i = 0; value[i]; i++) {
			if (isspace(value[i]))
				num = i;
		}
		value.erase(value.begin() + num);
		locStruct.path = value;

	}
	else if (key == "root")
		locStruct.root = value;
	else if (key == "client_max_body_size")
	{
		uint64_t maxSize;

		maxSize = strToNum(value);
		if (maxSize > UINT64_MAX) //? mac printed out 'error', not sure if linux will do the same
			return false;
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
		locStruct.retur.code = 0;
		if (!value.empty())
		{
			short int status = 0;

			locStruct.retur.have = true;
			status = strToShortInt(valueVec[0]);
			if (status < 100 || status > 599)
			{
				std::cout << "Return status must not be less than 1xx or more than 5xx" << std::endl;
				return false;
			}
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
	return true;
}

bool setValue(Server &obj, Location &locStruct, std::string key, std::string value, bool isLocation)
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
	{
		if (!storeLocation(locStruct, key, value, valueVec))
			return false;
	}
	else
	{
		if (!storeDirectives(obj, key, value, valueVec))
			return false;
	}
	return true;
}

int getLocation(Server &obj, Location &locStruct, std::string key, std::string value, bool &isLocation)
{
	static int firstLoc = 0;

	if (key != "location" && !isLocation)
		return 0;
	// remove { from the path
	for (size_t i = 0; i < value.length() && key == "location"; i++)
	{
		if (value[i] == '{')
			value.erase(value.begin() + i);
	}
	isLocation = 1;
	if (key[0] == '}') // when read the line with } will result in key with length of 2. Therefore, use char as a condition
	{
		isLocation = 0;
		firstLoc = 0;
		obj.location.push_back(locStruct);
		locStruct = clearLocation(obj);
		return 0;
	}
	// set location values to default as according to the server
	if (firstLoc == 0)
	{
		locStruct = clearLocation(obj);
		firstLoc = 1;
	}
	if (!setValue(obj, locStruct, key, value, 1))
		return 2;
	return 1;
}

std::string getValue(std::string value, std::string key, std::string line, int &i)
{
	while (line[i] && key != "server")
	{
		while (isspace(line[i]))
			i++;
		// copy value(s)
		while (line[i])
		{
			if (line[i] == ';' || line[i] == '#')
			{
				i++;
				break;
			}
			value += line[i++];
		}
	}
	// remove ; and /r from the value
	for (int j = 0; j < value[j]; j++) //? we could use find() here
	{
		if (value[j] == '\r')
			value.erase(value.begin() + j);
		// if (value[j] == ';' || value[j] == '\r')
		// 	value.erase(value.begin() + j);
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
// 	Server obj;
// 	Location locStruct;
// 	std::vector<Server> servers;
// 	bool isLocation = false;
// 	bool firstServer = true;

// 	if (ac != 2)
// 		return (std::cout << "must have 2 arguments" << std::endl, 0);
// 	configFile.open(av[1]);
// 	if (!configFile.is_open())
// 		return (std::cout << "cannot open config file" << std::endl, 0);
// 	while (std::getline(configFile, tmp))
// 	{
// 		int i = 0;
// 		int	isSetLocation = 0;
// 		std::string key;
// 		std::string value;

// 		key = getKey(key, tmp, i);
// 		if (key == "server" && firstServer)
// 		{
// 			firstServer = false;
// 			continue;
// 		}
// 		value = getValue(value, key, tmp, i);
// 		isSetLocation = getLocation(obj, locStruct, key, value, isLocation);
// 		if (isSetLocation == 1)
// 			continue;
// 		else if (isSetLocation == 2)
// 			return 0;
// 		if (!setValue(obj, locStruct, key, value, 0))
// 			return 0;
// 		if (key == "server" || configFile.peek() == EOF)
// 		{
// 			servers.push_back(obj);
// 			clearServer(obj);
// 		}
// 	}
// 	if (!scanPorts(servers))
// 		return 0;
// 	// exit (0);
// 	printServers(servers);
// 	// printConfig(obj);
// }
