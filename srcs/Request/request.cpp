/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sahafid <sahafid@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/01 10:41:11 by houazzan          #+#    #+#             */
/*   Updated: 2023/04/08 23:45:28 by sahafid          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/request.hpp"
#include "../../includes/config.hpp"

Request::Request(const std::string& str) :
 _methode(""), _version(""), _body(""), _path(""), _port(80), _ret(200)
{
	tokenizingAndParsing(str);
}

/* *************************************************************************** */

Request::~Request(){}

/* *************************************************************************** */

int Request::get_ret()
{
	return (_ret);
}

int Request::get_port()
{
	return (_port);
}

std::string Request::get_methode()
{
	return(_methode);
}

std::string Request::get_version()
{
	return (_version);
}

std::string Request::get_body()
{
	return(_body);
}

std::string Request::get_path()
{
	return (_path);
}


std::string Request::get_query()
{
	return (_query);
}


std::string Request::get_header(std::string str)
{
	return (_header[str]);
}


