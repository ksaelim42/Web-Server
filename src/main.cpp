/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prachman <prachman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 11:10:00 by prachman          #+#    #+#             */
/*   Updated: 2024/02/11 15:30:14 by prachman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpReq.hpp"

int main()
{
	// // // the string has 14 newline characters
	std::string rawReq = "GET /favicon.ico HTTP/1.1\nHost: localhost:8080\nConnection: keep-alive\nsec-ch-ua: ""Not A(Brand"";v=""99"", ""Google Chrome"";v=""121"", ""Chromium"";v=""121""\nsec-ch-ua-mobile: ?0\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/121.0.0.0 Safari/537.36\nsec-ch-ua-platform: ""Windows""\nAccept: image/avif,image/webp,image/apng,image/svg+xml,image/,/*;q=0.8\nSec-Fetch-Site: same-origin\nSec-Fetch-Mode: no-cors\nSec-Fetch-Dest: image\nReferer: http://localhost:8080/\nAccept-Encoding: gzip, deflate, br\nAccept-Language: en-US,en;q=0.9,th;q=0.8\n";
	// std::string rawReq = "hey\n ssdfsdf\n";
	std::vector<std::string> vecReq;
	for (int i = 0; rawReq[i]; i++)
	{
		std::string tmp;
		while (rawReq[i] != '\n')
		{
			tmp += rawReq[i++];
		}
		vecReq.push_back(tmp);
	}
	for (std::vector<std::string>::iterator it = vecReq.begin(); it != vecReq.end(); it++)
	{
		std::cout << *it << std::endl;
	}
}
