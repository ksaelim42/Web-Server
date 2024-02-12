/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpReq.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prachman <prachman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 11:10:04 by prachman          #+#    #+#             */
/*   Updated: 2024/02/12 14:36:53 by prachman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQUEST
#define HTTP_REQUEST

#include <iostream>
#include <string.h>

struct startLine
{
	std::string method;
	std::string srcPath;
	std::string version;
};

//sec_ua == user-agent
//mobile == indicates whether the user using mobile
//accept == what types of content that client can understand
//fetch_site == indicates where the request originated from
//referer == provides the URL of the page that referred the client to the current requests 
//language == specifies the preferred languages for the response
//!!  >>>>>> those that are not defined are the headers that I am not sure we need <<<<<
struct httpReq
{
	/* data */
	startLine	sLine;
	std::string	host;
	std::string	connection;
	std::string	sec_ua; 
	std::string	mobile; 
	std::string	ua;
	std::string	ua_platform;
	std::string	accept;
	std::string	fetch_site;
	std::string	fetch_mode;
	std::string	fetch_dest;
	std::string	referer;
	std::string	encoding;
	std::string	language;

};

#endif