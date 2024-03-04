/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prachman <prachman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 11:10:00 by prachman          #+#    #+#             */
/*   Updated: 2024/02/15 09:34:18 by prachman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpReq.hpp"

httpReq	storeReq(std::string rawReq)
{
	// // // the string has 14 newline characters
	std::vector<std::string>	headVec;
	std::vector<std::string>	tailVec;
	std::vector<std::string>	vecStartLine;
	httpReq 					reqData;
	int							count = 0;

	for (int i = 0; rawReq[i] != '\n'; i++) count++;
	for (int i = 0; i < count; i++)
	{
		std::string tmp;
		while (!isspace(rawReq[i]))
			tmp += rawReq[i++];
		vecStartLine.push_back(tmp);
	}
	reqData.method = vecStartLine[0];
	reqData.srcPath = vecStartLine[1];
	reqData.version = vecStartLine[2];
	for (int i = 0; rawReq[i]; i++)
	{
		std::string headTmp;
		if (rawReq[i - 1] == '\n')
		{
			while (rawReq[i] != ':')
				headTmp += rawReq[i++];
			headVec.push_back(headTmp);
		}
		std::string tailTmp;
		if (rawReq[i] == ':' && rawReq[i + 1] == ' ')
		{
			i += 2;
			while (rawReq[i] != '\r')
				tailTmp += rawReq[i++];
			tailVec.push_back(tailTmp);
			i++;
		}
	}
	if (headVec.size() != tailVec.size())
	{
		std::cout << "Error: Something is wrong when trying to store the request" << std::endl;
		exit(1);
	}
	for (int i = 0; i < headVec.size(); i++) 
		reqData.headers[headVec[i]] = tailVec[i];
	
	return reqData;
}


// int	scanStartLine(std::map<std::string, std::string> startLine)
// {
// 	std::string	methods[] = {"GET", "POST", "DELETE"};
// 	bool	isMethod = false;
	
// 	for (int i = 0; i < 3; i++)
// 	{
// 		if (startLine["method"] == methods[i])
// 			isMethod = true;
// 	}
// 	if (!isMethod)
// 		return (std::cout << "Incorrect HTTP Method: " << startLine["method"] << std::endl, 400);
// 	if (startLine["version"] != "HTTP/1.1")
// 		return (std::cout << "Incorrect HTTP version: " << startLine["version"] << std::endl, 400);
// 	return 0;
// }