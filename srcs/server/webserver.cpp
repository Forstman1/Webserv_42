/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserver.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rel-fagr <rel-fagr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/24 04:00:17 by garra             #+#    #+#             */
/*   Updated: 2023/02/13 21:30:20 by rel-fagr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/webserver.hpp"

void    webServer::setupServer()
{
    for (size_t i = 0; i < _serv.size(); i++)
    {
	    int optval = 1;

	    _serv[i].socket_fd = guard(socket(AF_INET, SOCK_STREAM, 0), "socket_fd error");
	    guard(setsockopt(_serv[i].socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)), "setsockopt error");
	   	guard(bind(_serv[i].socket_fd, (struct sockaddr *) &_serv[i]._address, sizeof(_serv[i]._address)), "error bind");
        guard(listen(_serv[i].socket_fd, BACKLOG), "listen error");
        guard(fcntl(_serv[i].socket_fd, F_SETFL, O_NONBLOCK), "fcntl error");

		fds_info fdtmp;
		
		fdtmp.tmp.fd = _serv[i].socket_fd;
	    fdtmp.tmp.events = POLLIN;
		fdtmp.total = 0;
		fdtmp.is_complet = false;
		fdtmp.str_header = "";
		fdtmp.is_first_time = true;
		fdtmp.content_length = 0;
	
		fdsInfo.push_back(fdtmp);
		fds.push_back(fdtmp.tmp);
		std::cout << "lestening to server " << _serv[i].server_name[0] << " host " << _serv[i].host << \
			" port " << _serv[i]._port << std::endl;
    }
	std::cout << std::endl;
	fds_len = fds.size();
    acceptConnection();
}

//--------------------------------------------------------------------------

int webServer::is_socket(int fd)
{
	for (size_t i = 0; i < _serv.size(); i++)
	{
		if (fd == _serv[i].socket_fd)
		{
			while((client_sockets = accept(server_sock,(struct sockaddr *) &client_address, &addrlen)) > 0)
			{
				guard(fcntl(client_sockets, F_SETFL, O_NONBLOCK), "fcntl error");
				fds_info fdtmp;
				
	    		fdtmp.tmp.fd = client_sockets;
	    		fdtmp.tmp.events = POLLIN;
				fdtmp.total = 0;
				fdtmp.is_complet = false;
				fdtmp.str_header = "";
				fdtmp.is_first_time = true;
				fdtmp.content_length = 0;

				fds.push_back(fdtmp.tmp);
				fdsInfo.push_back(fdtmp);
	    		fds_len++;
			}
			server_sock = fd;
			port = _serv[i]._port;
			return 1;
		}
	}
	return 0;
}


//--------------------------------------------------------------------------

int     webServer::Poll_in(int &i)
{
	if (is_socket(fds[i].fd))
		return 1;
	else
	{
        read_header(i);
        if ((fdsInfo[i].read_len <= -1 && errno != EAGAIN) || fdsInfo[i].read_len == 0)
		{
			printf("hello\n");
			if (fdsInfo[i].read_len == 0)
				perror("Client disconnected");
			close(fdsInfo[i].tmp.fd);
			fdsInfo.erase(fdsInfo.begin()+i);
			fds.erase(fds.begin()+i);
			i--;
			return 1;
		}
        else if (fdsInfo[i].is_complet == true)
        	fds[i].events = POLLOUT;
	}
	return 0;
}

//--------------------------------------------------------------------------

void     webServer::Poll_out(int i)
{
	std::fstream file;
	std::string line;
	std::string appendLine;
	std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length:";
	Servers my_server = FoundServer();
	int len;
	
	file.open(fileExemple, std::fstream::in);
    if (!file.is_open()) std::cout << "Can't open file!" << std::endl , std::exit(EXIT_FAILURE);
    while (!std::getline(file, line).eof())
	{
        appendLine.append(line);
	}

	len = appendLine.length();
	response.append(std::to_string(len));
	response.append("\n\n");
	response.append(appendLine);
	
	sendall(fdsInfo[i].tmp.fd, response, response.size());
	
	close(fdsInfo[i].tmp.fd);
	fdsInfo.erase(fdsInfo.begin()+i);
	fds.erase(fds.begin()+i);
	// std::cout << "-------- message sent --------" << std::endl;
}


//--------------------------------------------------------------------------


void    webServer::acceptConnection(void)
{
    while(1)
	{
		guard(poll(&fds[0], fds_len, 0), "poll error");
		for (int i = 0; i < fds_len; i++)
		{
			if (fds[i].revents & POLLIN)
			{
				if(Poll_in(i))
					continue;
			}
			else if (fds[i].revents & POLLOUT)
				Poll_out(i);
			// else if (fds[i].revents & (POLLHUP | POLLERR))
			// {
			// 	perror("Connection error with client");
    		// 	close(fds[i].fd);
			// 	reset_my_fd(my_fd, i);
    		// 	i--;
            // 	continue;
			// }
		}
	}
    for (int i = 0; i < fds_len; i++)
        close(fds[i].fd);
}

//--------------------------------------------------------------------------

int webServer::guard(int n, const char *er)
{
    if (n < 0)
    {
        perror(er);
        exit(EXIT_FAILURE);
    }
    return n;
}

//--------------------------------------------------------------------------

webServer::webServer(std::vector<Servers> servers)
{
    for (size_t i = 0; i < servers.size(); i++)
    {
        for(size_t j = 0; j < servers[i].port.size(); j++)
        {
            Servers _servers = servers[i];
            _servers._port = servers[i].port[j];
            _servers._address.sin_family = AF_INET;
            _servers._address.sin_addr.s_addr = inet_addr(servers[i].host.c_str());
            _servers._address.sin_port = htons(servers[i].port[j]);
            memset(_servers._address.sin_zero, '\0', sizeof(_servers._address.sin_zero));
            this->_serv.push_back(_servers);
        }
    }
}

webServer::~webServer(){}

//--------------------------------------------------------------------------

fds_info webServer::FoundFd(int fd)
{
	size_t j = 0;
	for (; j < fdsInfo.size(); j++)
	{
		if(fd == fdsInfo[j].tmp.fd)
			break;
	}
	std::cout << "j = " << j << std::endl; 
	return fdsInfo[j];
}

//--------------------------------------------------------------------------

Servers  webServer::FoundServer()
{
	size_t j = 0;
	for (; j < _serv.size(); j++)
	{
		if(_serv[j].socket_fd == server_sock && _serv[j]._port == port)
			break;
	}
	return _serv[j];
}

//--------------------------------------------------------------------------


void webServer::read_header(int i)
{
	Servers my_server = FoundServer();
    char buffer[50000];
    memset(buffer, '\0', sizeof(buffer));

	fdsInfo[i].read_len = 0;
    fdsInfo[i].read_len = read(fdsInfo[i].tmp.fd, buffer, sizeof(buffer));
	if (fdsInfo[i].read_len > 0)
	{
    	std::string str(buffer, fdsInfo[i].read_len);
    	fdsInfo[i].str_header += str;
    	fdsInfo[i].total += fdsInfo[i].read_len;
		if (fdsInfo[i].total > my_server.client_max_body_size && my_server.client_max_body_size != 0)
		{
			fdsInfo[i].str_header = "";
			std::cerr << "server 413 Request Entity Too Large" << std::endl;
			return ;
		}
		// fdsInfo[i].str_header = "";
    }
	std::cout << "str_header = "<< fdsInfo[i].str_header << std::endl;
	std::cout <<"is_complet = " << fdsInfo[i].is_complet<<std::endl;
	std::cout <<"is_first_time = " << fdsInfo[i].is_first_time<<std::endl;
	std::cout <<"read_len = " << fdsInfo[i].read_len<<std::endl;
	std::cout <<"total = " << fdsInfo[i].total<<std::endl;
	std::cout << "--------------------------------------------" << std::endl;
	fdsInfo[i].is_complet = true;
}

//--------------------------------------------------------------------------


void webServer::sendall(int fd, std::string response, int len)
{
    int total = 0;
    int bytesleft = len;
    int n;
    while(total < len)
    {
        n = send(fd, response.c_str()+total, bytesleft, 0);
        if (n == -1)
            break;
        total += n;
        bytesleft -= n;
    }
}