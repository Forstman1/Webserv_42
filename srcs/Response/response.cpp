/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ouzhamza <ouzhamza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/12 00:52:50 by ouzhamza          #+#    #+#             */
/*   Updated: 2023/03/29 16:36:03 by ouzhamza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/response.hpp"


Response::Response(Request &_request,  fds_info &_fd) : request(_request), fd(_fd), server(choseServer())
{
	// if (_fd.isTimeOut == true)
	// 	std::cout << _version << std::endl;
	initErrorMap();
	initRespMaps();
	initContentMap();
	setBool();
	setPath();

}


/* ************************************************************************** */

Response::~Response(){}

Response::Cgi::Cgi(){}

Response::Cgi::~Cgi(){}

/* ************************************************************************** */

Servers& Response::choseServer()
{
	// server = servers[0];
	// std::cout << request.get_header("Host") << std::endl;
	// std::cout << request.get_path() << std::endl;
	return(fd.my_servers[0]);
}

/* ************************************************************************** */

void	Response::initErrorMap()
{
	_status_code[100] = "Continue";
	_status_code[200] = "OK";
	_status_code[201] = "Created";
	_status_code[202] = "Accepted";
	_status_code[204] = "No Content";
	_status_code[300] = "Multiple Choices";
	_status_code[301] = "Moved Permanently";
	_status_code[302] = "Found";
	_status_code[303] = "See Other";
	_status_code[304] = "Not Modified";
	_status_code[305] = "Use Proxy";
	_status_code[307] = "Temporary Redirect";
	_status_code[400] = "Bad Request";
	_status_code[403] = "Forbidden";
	_status_code[404] = "Not Found";
	_status_code[405] = "Method Not Allowed";
	_status_code[413] = "Payload Too Large";
	_status_code[500] = "Internal Server Error";
}


void Response::setBool()
{
	_index = false;
	_cgi   = false;
}
/* ************************************************************************** */
void	Response::initRespMaps()
{
	_header["Date"] = "Date: ";
	_header["Connection"] = "Connection: ";
	_header["Content-Type"] = "Content-Type: ";
	_header["Content-Length"] = "Content-Length: ";
	_header["Location"] = "Location: ";
	// _header["Cache-Control"] = "";
	// _header["Transfer-Encoding"] = "";
}

/* ************************************************************************** */

void	Response::initContentMap()
{
	std::ifstream file;
	std::string line;
	
	file.open("srcs/Conf/mime.types");
	if (file.is_open())
	{
		while (getline(file, line))
			_contentType[get_key(line)] = get_value(line);
		file.close();
	}
	else
		std::cout << "Can't open mime.types file" << std::endl;
}

/* ************************************************************************** */

std::string Response::call()
{
	if (_ret != 200)
		error();
	else {
	_ret = ParsingResponse();
	if (_cgi)
		runcgi();
	else if (_ret != 200)
		error();
	if(_methode == "GET")
		getMethode();
	else if (_methode == "POST")
		postMethode();
	else if (_methode == "DELETE")
		deleteMethode();
	}
	headerGen();
	return (_response);
}


/* ************************************************************************** */

void	Response::setPath()
{
	if (server.root[server.root.length() - 1] == '/')
		server.root.erase(server.root.length() - 1);
	this->_path = request.get_path();
	_path = decodePath();
	_root = server.root;
	setVersion();
}


std::string Response::decodePath()
{
	std::stringstream output;
	char decoded[3];
	std::string var;
	for (std::string::const_iterator it = _path.begin(); it != _path.end(); it++) {
		if (*it == '%')
		{
			decoded[0] = *(++it);
			decoded[1] = *(++it);
			var  = std::strtol(decoded, NULL, 16);
			output << var;
		}
		else if (*it == '+')
            output << ' ';
         else 
            output << *it;
	}
	return (output.str());
}

void	Response::setVersion()
{
	this->_version = "HTTP/1.1 ";
	setQuery();
}

void	Response::setQuery()
{
	this->_query = request.get_query();
	setRet();
}

void	Response::setRet()
{
	this->_ret = request.get_ret();
	setMethode();
}


void	Response::setMethode()
{
	this->_methode = request.get_methode();
	setHost();
}

void	Response::setHost()
{
	this->_host = request.get_header("Host");
	setPort();
}

void	Response::setPort()
{
	this->_port = request.get_port();
	setBody();
}

void	Response::setBody()
{
	this->_Reqbody = request.get_body();
}

std::string Response::get_key(std::string line)
{
	size_t i = line.find(":");
	return (line.substr(i + 1));
}

std::string Response::get_value(std::string line)
{
	size_t i = line.find(":");
	return (line.substr(0, i));	
}
