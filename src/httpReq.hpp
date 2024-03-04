/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpReq.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prachman <prachman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 11:10:04 by prachman          #+#    #+#             */
/*   Updated: 2024/03/04 13:17:34 by prachman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQUEST
#define HTTP_REQUEST

#include <iostream>
#include <map>
#include <string.h>

struct httpReq
{
	std::map<std::string, std::string> startLine;
	std::map<std::string, std::string> headers;
};

#endif