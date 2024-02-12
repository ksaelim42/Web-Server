/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prachman <prachman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 11:10:00 by prachman          #+#    #+#             */
/*   Updated: 2024/02/12 15:02:13 by prachman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpReq.hpp"

int main()
{
	// // // the string has 14 newline characters
	std::string					rawReq = "GET /favicon.ico HTTP/1.1\nHost: localhost:8080\nConnection: keep-alive\nsec-ch-ua: ""Not A(Brand"";v=""99"", ""Google Chrome"";v=""121"", ""Chromium"";v=""121""\nsec-ch-ua-mobile: ?0\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/121.0.0.0 Safari/537.36\nsec-ch-ua-platform: ""Windows""\nAccept: image/avif,image/webp,image/apng,image/svg+xml,image/,/*;q=0.8\nSec-Fetch-Site: same-origin\nSec-Fetch-Mode: no-cors\nSec-Fetch-Dest: image\nReferer: http://localhost:8080/\nAccept-Encoding: gzip, deflate, br\nAccept-Language: en-US,en;q=0.9,th;q=0.8\n";
	std::vector<std::string>	vecReq;
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
	reqData.sLine.method = vecStartLine[0];
	reqData.sLine.srcPath = vecStartLine[1];
	reqData.sLine.version = vecStartLine[2];
	for (int i = 0; rawReq[i]; i++)
	{
		std::string tmp;
		if (rawReq[i] == ':' && rawReq[i + 1] == ' ')
		{
			i += 2;
			while (rawReq[i] != '\n')
				tmp += rawReq[i++];
			vecReq.push_back(tmp);
		}
	}
	reqData.host = vecReq[0];
	reqData.connection = vecReq[1];
	reqData.sec_ua = vecReq[2];
	reqData.mobile = vecReq[3];
	reqData.ua = vecReq[4];
	reqData.ua_platform = vecReq[5];
	reqData.accept = vecReq[6];
	reqData.fetch_site = vecReq[7];
	reqData.fetch_mode = vecReq[8];
	reqData.fetch_dest = vecReq[9];
	reqData.referer = vecReq[10];
	reqData.encoding = vecReq[11];
	reqData.language = vecReq[12];

	// for (std::vector<std::string>::iterator it = vecStartLine.begin(); it != vecStartLine.end(); it++)
	// {
	// 	std::cout << *it << std::endl;
	// }
}
