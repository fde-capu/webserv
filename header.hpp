/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/18 19:19:04 by fde-capu          #+#    #+#             */
/*   Updated: 2022/09/27 20:12:53 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_HPP
# define HEADER_HPP

# define WS_DEFAULT_CONFIG "webserv-default.conf"
# define WS_ARGVAL "argval_ws.conf"
# define WSCGI_ARGVAL "argval_cgi.conf"

# include <map>
# include <string>
# include <iostream>
# include <cstdlib>
# include <fstream>
# include "bladefs.hpp"
# include "StringTools.hpp"
# include "FileString.hpp"
# include "ArgVal.hpp"
# include "DataFold.hpp"
# include "CircularBuffer.hpp"
# include "Chronometer.hpp"
# include "TemplateError.hpp"
# include <algorithm>
# include <vector>
# include <arpa/inet.h>
# include <netdb.h>
# include <unistd.h>
# include <poll.h>
# include <fcntl.h>
# include <sys/types.h>
# include <dirent.h>

# ifndef VERBOSE
#  define VERBOSE 0
# endif
# define VERBOSE_HEADER 3

# define VLIM 60

# define DEFAULT_INDEX "bliblib.html boblob.htm"
# define DEFAULT_ACCEPTED_METHODS "GET"
# define DEFAULT_SERVER_NAME "fde-capu's webserv"
# define DEFAULT_WELCOME_MESSAGE "Welcome!"
# define DEFAULT_BYE "Bye!"
# define DEFAULT_MAX_BODY_SIZE 0
# define TIME_OUT_MSEC 2500

typedef std::map<std::string, std::string> maps;
typedef DataFold::vint vint;
typedef DataFold::vstr vstr;

void remove_comments(std::string& line);
bool valid_line(std::string line);
std::string get_parameter(std::string line);
std::string get_value(std::string line);

# define SERVER_UP "Server is up."
# define SERVER_DN "Server is dn."

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
