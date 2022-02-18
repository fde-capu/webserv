/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defines.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/06 02:52:43 by fde-capu          #+#    #+#             */
/*   Updated: 2022/02/18 20:59:14 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFINES_HPP
# define DEFINES_HPP

# define STATUS_200 "HTTP/1.1 200 OK"
# define RUNNING << "Server is up." << std::endl;
# define TURN_OFF << "Server is dn." << std::endl;
# define NL << std::endl;

# define CONF_DELIMITERS " \t"
# define TRIM_CHARACTERS " \t"
# define PARAMETER_ALLOWED_CHARACTERS "abcdefghijklmnopqrstuvwxyz_0123456789"
# define COMMENT_CHAR '#'

# define ERR "(webserv) Error: "
# define ERR_SEGUE " : "
# define ERR_INVALID_ARGS "Invalid arguments."
# define ERR_INVALID_FILE "Invalid file."
# define ERR_INVALID_PARAM "Invalid parameter."

#endif
