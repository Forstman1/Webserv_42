/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sahafid <sahafid@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/10 16:57:09 by sahafid           #+#    #+#             */
/*   Updated: 2023/04/09 00:57:07 by sahafid          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../../includes/config.hpp"
#include "../../../includes/response.hpp"
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <fcntl.h>
#include <sys/wait.h>



char    **setEnv(Response::Cgi cgi, std::string fileName)
{
    char **envirement = (char**)calloc(13 , sizeof(char*));
    
    std::string variable;
    variable = "CONTENT_LENGTH=";
    variable.append(cgi.getCgicontentLength());
    envirement[0] = strdup(variable.c_str());
    
    variable = "CONTENT_TYPE=";
    variable.append(cgi.getCgicontentType());
    envirement[1] = strdup(variable.c_str());
    
    variable = "QUERY_STRING=";
    if (cgi.getCgiMethode() != "POST")
        variable.append(cgi.getCgiQuery());
    envirement[2] = strdup(variable.c_str());  

    variable = "DOCUMENT_ROOT=";
    variable.append(cgi.getCgiroot());
    envirement[3] = strdup(variable.c_str());

    
    variable = "PATH_INFO=";
    variable.append(cgi.getCgiPath());
    envirement[4] = strdup(variable.c_str());

    variable = "REQUEST_METHOD=";
    variable.append(cgi.getCgiMethode());
    envirement[5] = strdup(variable.c_str());
    
    variable = "SERVER_PORT=";
    variable.append(cgi.getCgiPort());
    envirement[6] = strdup(variable.c_str());

    variable = "SERVER_NAME=";
    variable.append(cgi.getCgiServerName());
    envirement[7] = strdup(variable.c_str());

    variable = "REDIRECT_STATUS=";
    variable.append(cgi.getCgiredirectStatus());
    envirement[8] = strdup(variable.c_str());


    variable = "SERVER_PROTOCOL=";
    variable.append(cgi.getCgiserverProtocol());
    envirement[9] = strdup(variable.c_str());


    variable = "HTTP_COOKIE=";
    variable.append(cgi.getCoockies());
    envirement[10] = strdup(variable.c_str());

    variable = "SCRIPT_FILENAME=";
    variable.append(fileName);
    envirement[11] = strdup(variable.c_str());
    
    envirement[12] = NULL;
    
    return envirement;
}




std::string    Response::executeCgi(std::string fileName, Cgi cgi)
{
    
    fileName = cgi.getCgiroot() + fileName;
    std::ifstream check;
    check.open(fileName);

    if (!check.is_open())
    {
        remove("./tmpFile");
        throw std::invalid_argument("missing lines");
    }
    
    std::string cmd = server.locations[_l].fatscgi_pass;
    
    remove("./tmpFile");
    
    int fd = open("./tmpFile", O_CREAT | O_WRONLY | O_TRUNC);
    int pid = fork();
    
    if (pid == 0)
    {
        int fds[2];
        pipe(fds);
        
        dup2(fd, 1);
        close(fd);
        
        if (cgi.getCgiMethode() == "POST")
        {
            write(fds[1], cgi.getCgiQuery().c_str(), cgi.getCgiQuery().length());
            dup2(fds[0], 0);
        }
        close(fds[0]);
        close(fds[1]);
        char *argv[3];
        argv[0] = strdup((char*)cmd.c_str());
        argv[1] = strdup((char *)fileName.c_str());
        argv[2] = NULL;

        char **envp = setEnv(cgi, fileName);
        execve(argv[0], argv, envp);
        exit(0);
    }

    waitpid(-1, NULL, 0);
    close(fd);

    
    std::string tmp;
    std::string lines;
    std::vector<std::string> alllines;
    
    std::ifstream test;

    test.open("./tmpFile");
    
    
    while(getline(test, tmp))
    {
        alllines.push_back(tmp + "\n");
    }
    
    cgi_line = "";
    if (alllines.size() > 2)
    {
        std::vector<std::string>::iterator iter = alllines.begin();
        
        while (iter != alllines.end())
        {
            if ( (*iter)[0] == 13 || (*(iter +1))[0] == 0 || (*(iter))[0] == '\n')
            {
                if (alllines.size() > 1)
                    iter = alllines.erase(iter);
                if (alllines.size() > 1)
                    iter = alllines.erase(iter);
                break ;
            }
            cgi_line += *iter;

            iter = alllines.erase(iter);
        }
        for (std::vector<std::string>::iterator it = alllines.begin(); it != alllines.end(); it++)
        {
            lines.append(*it);
        }
    }
    else
    {
        remove("./tmpFile");
        throw std::invalid_argument("missing lines");
    }

    remove("./tmpFile");
    return lines;
}
