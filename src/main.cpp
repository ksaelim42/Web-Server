/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prachman <prachman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 11:10:00 by prachman          #+#    #+#             */
/*   Updated: 2024/02/23 13:25:23 by prachman         ###   ########.fr       */
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
			while (rawReq[i] != '\n')
				tailTmp += rawReq[i++];
			tailVec.push_back(tailTmp);
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

int	main()
{
	httpReq		repData;
	std::string	rawReq = "GET /favicon.ico HTTP/1.1\nHost: localhost:8080\nConnection: keep-alive\nsec-ch-ua: ""Not A(Brand"";v=""99"", ""Google Chrome"";v=""121"", ""Chromium"";v=""121""\nsec-ch-ua-mobile: ?0\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/121.0.0.0 Safari/537.36\nsec-ch-ua-platform: ""Windows""\nAccept: image/avif,image/webp,image/apng,image/svg+xml,image/,/*;q=0.8\nSec-Fetch-Site: same-origin\nSec-Fetch-Mode: no-cors\nSec-Fetch-Dest: image\nReferer: http://localhost:8080/\nAccept-Encoding: gzip, deflate, br\nAccept-Language: en-US,en;q=0.9,th;q=0.8\n";
	repData= storeReq(rawReq);

	std::cout << repData.method << std::endl;
	std::cout << repData.srcPath << std::endl;
	std::cout << repData.version << std::endl;
	for (std::map<std::string, std::string>::iterator it = repData.headers.begin(); it != repData.headers.end(); it++)
		std::cout << it->first << " => " << it->second << std::endl;
	
}