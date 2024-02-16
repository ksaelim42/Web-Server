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
	reqData.method = vecStartLine[0];
	reqData.srcPath = vecStartLine[1];
	reqData.version = vecStartLine[2];
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
	//sec_ua == user-agent
	//mobile == indicates whether the user using mobile
	//accept == what types of content that client can understand
	//fetch_site == indicates where the request originated from
	//referer == provides the URL of the page that referred the client to the current requests 
	//language == specifies the preferred languages for the response
	//!!  >>>>>> those that are not defined are the headers that I am not sure we need <<<<<
	reqData.headers["Host"] = vecReq[0];
	reqData.headers["Connection"] = vecReq[1];
	// reqData.headers["sec-ch-ua"] = vecReq[2];
	// reqData.headers["sec-ch-mobile"] = vecReq[3];
	// reqData.headers["User-Agent"] = vecReq[4];
	// reqData.headers["sec-ch-ua-platform"] = vecReq[5];
	// reqData.headers["Accept"] = vecReq[6];
	// reqData.headers["Sec-Fetch-Site"] = vecReq[7];
	// reqData.headers["Sec-Fetch-Mode"] = vecReq[8];
	// reqData.headers["Sec-Fetch-Dest"] = vecReq[9];
	// reqData.headers["Referer"] = vecReq[10];
	// reqData.headers["Accept-Encoding"] = vecReq[11];
	// reqData.headers["Accept-Language"] = vecReq[12];
	std::cout << "store Request Success" << std::endl;
	return reqData;
}