/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpReq.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prachman <prachman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 11:10:04 by prachman          #+#    #+#             */
/*   Updated: 2024/02/15 12:17:58 by prachman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQUEST
#define HTTP_REQUEST

#include <iostream>
#include <map>
#include <string.h>

struct httpReq
{
	std::string method;
	std::string srcPath;
	std::string version;
	std::map<std::string, std::string> headers;
};

#endif