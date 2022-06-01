/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/18 19:19:04 by fde-capu          #+#    #+#             */
/*   Updated: 2022/06/01 16:31:41 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_HPP
# define HEADER_HPP

# define WS_DEFAULT_CONFIG "webserv-default.conf"
# define WS_ARGVAL "argval.conf"
# define WSCGI_ARGVAL "argval_cgi.conf"

# include <map>
# include <string>
# include <iostream>
# include <cstdlib>
# include <fstream>
# include "bladefs.hpp"
# include "FileString.hpp"
# include "ArgVal.hpp"
# include "DataFold.hpp"
# include "CircularBuffer.hpp"
# include <algorithm>
# include <iostream>
# include <vector>
# include <arpa/inet.h>
# include <netdb.h>
# include <unistd.h>
# include <poll.h>
# include <fcntl.h>
# include <sys/wait.h>

# ifndef VERBOSE
#  define VERBOSE 0
# endif

typedef std::map<std::string, std::string> maps;
typedef DataFold::vint vint;
typedef DataFold::vstr vstr;

bool die(std::string msg);
void remove_comments(std::string& line);
bool valid_line(std::string line);
void alert(std::string msg);
void alert(std::string msg, std::string &line);
std::string get_parameter(std::string line);
std::string get_value(std::string line);

# define STATUS_200 "HTTP/1.1 200 OK"

# define SERVER_UP "Server is up."
# define SERVER_DN "Server is dn."
# define DEFAULT_CONFIG_FILE "webserv-default.conf"
# define DEFAULT_SERVER_NAME ""
# define NL << std::endl;

# define CONF_DELIMITERS " \t"
# define TRIM_CHARACTERS " \t"
# define PARAMETER_ALLOWED_CHARACTERS "abcdefghijklmnopqrstuvwxyz_0123456789"
# define COMMENT_CHAR '#'

# define ALERT "(webserv)"
# define ALERT_SEGUE " "
# define ERROR "Error: "
# define ERR_INVALID_ARGS ERROR "Invalid arguments."
# define ERR_INVALID_FILE ERROR "Invalid file."
# define ERR_INVALID_PARAM ERROR "Invalid parameter."
# define ERR_INCOMPLETE_SETUP ERROR "Incomplete setup."
# define ERR_SYNTAX ERROR "Syntax error."
# define ERR_FAILED_ADDRINFO "Failed addrinfo."
# define CONFIG_OK "Configuration ok."
# define CONFIG_CHECKING "Checking configuration file."
# define BYE "Bye!"

#endif
