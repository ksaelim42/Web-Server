/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prachman <prachman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 11:10:04 by prachman          #+#    #+#             */
/*   Updated: 2024/04/17 20:20:59 by prachman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQUEST
#define HTTP_REQUEST

#include <iostream>
#include <map>
#include <vector>
#include <string.h>
#include <cstdio>	// remove()
#include <cstdlib>

struct httpReq
{
	std::string method;
	std::string srcPath;
	std::string version;
	std::string	body;
	std::map<std::string, std::string> headers;
};

bool	storeReq(std::string rawReq, httpReq &reqData);

#endif