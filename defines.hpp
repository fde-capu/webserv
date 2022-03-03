/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defines.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/06 02:52:43 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/03 09:23:08 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFINES_HPP
# define DEFINES_HPP

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
# define CONFIG_OK "Configuration ok."
# define CONFIG_CHECKING "Checking configuration file."
# define BYE "Bye!"

#endif
