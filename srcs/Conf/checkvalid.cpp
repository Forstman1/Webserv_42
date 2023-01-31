/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checkvalid.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: garra <garra@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/25 19:43:04 by sahafid           #+#    #+#             */
/*   Updated: 2023/01/31 12:14:29 by garra            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/config.hpp"



void    checkFile(std::vector<errorPages> &error_pages)
{
    for (std::vector<errorPages>::iterator it = error_pages.begin(); it != error_pages.end(); it++)
    {
        std::ifstream file;
        file.open((*it).path.c_str());
        if (!file)
        {
            file.open((const char *)("/Users/sahafid/Desktop/webserv" + (*it).path).c_str());
            if (!file)
                (*it).path = "./error/";
        }
    }
}

void    checkDataValidity(Servers &server)
{
    std::vector<int> ports;
    if (server.port.size() < 1)
        throw std::invalid_argument("invalid input: no port number");
    if (server.host.empty())
        throw std::invalid_argument("invalid input: no host");
    for (std::vector<errorPages>::iterator it = server.error_page.begin(); it != server.error_page.end(); it++)
    {
        if (std::find(ports.begin(), ports.end(), (*it).status_code) != ports.end())
            throw std::invalid_argument("Syntax error: duplicate status code");
        ports.push_back((*it).status_code);
    }
    checkFile(server.error_page);
}