/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prachman <prachman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 11:10:04 by prachman          #+#    #+#             */
/*   Updated: 2024/03/04 21:20:59 by prachman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQUEST
#define HTTP_REQUEST

#include <iostream>
#include <map>
#include <vector>
#include <string.h>

struct httpReq
{
	std::string method;
	std::string srcPath;
	std::string version;
	std::string	body;
	std::map<std::string, std::string> headers;
};

httpReq	storeReq(std::string rawReq);

#endif