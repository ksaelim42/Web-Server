/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prachman <prachman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 11:10:00 by prachman          #+#    #+#             */
/*   Updated: 2024/03/07 12:35:15 by prachman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

httpReq	storeReq(std::string rawReq)
{
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
			// if this line is not present
			// the program will also store the unrelated data 
			// and result in unequal of field and value of headers
			if (rawReq[i + 1] != '\n')
			{
				while (rawReq[i] != ':')
					headTmp += rawReq[i++];
				headVec.push_back(headTmp);
			}
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
		std::cout << "Error: Something went wrong when trying to store the request" << std::endl;
		exit(1); //should not be exit but return 400
	}
	for (size_t i = 0; i < headVec.size(); i++) 
		reqData.headers[headVec[i]] = tailVec[i];
	
	return reqData;
}

int	scanStartLine(httpReq reqData)
{
	std::string	methods[] = {"GET", "POST", "DELETE"};
	bool	isMethod = false;
	
	for (int i = 0; i < 3; i++)
	{
		if (reqData.method == methods[i])
			isMethod = true;
	}
	if (!isMethod)
		return (std::cout << "Incorrect HTTP Method: " << reqData.method << std::endl, 400);
	if (reqData.version != "HTTP/1.1")
		return (std::cout << "Incorrect HTTP version: " << reqData.version << std::endl, 400);
	return 0;
}

// int	main()
// {
// 	httpReq		reqData;
	// int			status;

	// std::string	rawReq = "GET /favicon.ico HTTP/1.1\r\nHost: localhost:8080\r\nConnection: keep-alive\r\nsec-ch-ua: ""Not A(Brand"";v=""99"", ""Google Chrome"";v=""121"", ""Chromium"";v=""121""\r\nsec-ch-ua-mobile: ?0\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/121.0.0.0 Safari/537.36\r\nsec-ch-ua-platform: ""Windows""\r\nAccept: image/avif,image/webp,image/apng,image/svg+xml,image/,/*;q=0.8\r\nSec-Fetch-Site: same-origin\r\nSec-Fetch-Mode: no-cors\r\nSec-Fetch-Dest: image\r\nReferer: http://localhost:8080/\r\nAccept-Encoding: gzip, deflate, br\r\nAccept-Language: en-US,en;q=0.9,th;q=0.8\r\n\r\n";
// 	reqData= storeReq(rawReq);
// 	status = scanStartLine(reqData);
// 	// if (!status)
// 	// 	std::cout << "ok!" << std::endl;
// 	// else
// 	// 	std::cout << status << std::endl;
// 	// std::cout << reqData.method << std::endl;
// 	// std::cout << reqData.srcPath << std::endl;
// 	// std::cout << reqData.version << std::endl;
// 	// for (std::map<std::string, std::string>::iterator it = reqData.headers.begin(); it != reqData.headers.end(); it++)
// 	// 	std::cout << it->first << " => " << it->second << std::endl;
// }