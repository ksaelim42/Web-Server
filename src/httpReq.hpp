/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpReq.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prachman <prachman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 11:10:04 by prachman          #+#    #+#             */
/*   Updated: 2024/02/15 09:33:56 by prachman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQUEST
#define HTTP_REQUEST

#include <iostream>
#include <map>
#include <string.h>

//sec_ua == user-agent
//mobile == indicates whether the user using mobile
//accept == what types of content that client can understand
//fetch_site == indicates where the request originated from
//referer == provides the URL of the page that referred the client to the current requests 
//language == specifies the preferred languages for the response
//!!  >>>>>> those that are not defined are the headers that I am not sure we need <<<<<
struct httpReq
{
	std::string method;
	std::string srcPath;
	std::string version;
	std::map<std::string, std::string> headers;
};

#endif