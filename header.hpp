/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/18 19:19:04 by fde-capu          #+#    #+#             */
/*   Updated: 2022/02/21 16:51:43 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_HPP
# define HEADER_HPP

# include <map>
# include <string>
# include "defines.hpp"
# include <iostream>
# include <cstdlib>
# include <fstream>

# ifndef VERBOSE
#  define VERBOSE 0
# endif

typedef std::map<std::string, std::string> maps;
bool die(std::string msg);
void remove_comments(std::string& line);
void code_minimize(std::string& str);
bool valid_line(std::string line);
void alert(std::string msg);
void alert(std::string msg, std::string &line);
std::string get_parameter(std::string line);
std::string get_value(std::string line);

#endif
