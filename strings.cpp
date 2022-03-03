/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strings.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/21 16:42:34 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/03 09:22:05 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.hpp"

bool die(std::string msg)
{
	std::cout << ALERT << ALERT_SEGUE << msg << std::endl;
	return false;
}

void alert(std::string msg)
{
	std::cout << ALERT << ALERT_SEGUE << msg << std::endl;
}

void alert(std::string msg, std::string &line)
{
	std::cout << ALERT << ALERT_SEGUE << msg << ALERT_SEGUE << line << std::endl;
}

void show_file(std::fstream &file)
{
	std::string line;
	std::cout << std::endl << file << " ::::::::" << std::endl;
	while (std::getline(file, line))
		std::cout << line << std::endl;
	std::cout << ":::::::::::::::::::::::" << std::endl << std::endl;
}

std::string trim_lr(std::string &str, char x)
{
	while (str[0] == x)
		str.erase(0, 1);
	while (str[str.length() - 1] == x)
		str.erase(str.length() - 1, 1);
	return str;
}

std::string trim_lr_set(std::string str, std::string set)
{
	for (std::string::iterator it = set.begin(); it != set.end(); it++)
		trim_lr(str, *it);
	return str;
}

void code_minimize(std::string& str)
{
	str = trim_lr_set(str, TRIM_CHARACTERS);
}

bool is_in_set(char x, std::string set)
{
	for (std::string::iterator it = set.begin(); it != set.end(); it++)
	{
		if (*it == x)
			return true;
	}
	return false;
}

std::string get_parameter(std::string line)
{
	std::string::iterator it = line.begin();
	while (is_in_set(*it, PARAMETER_ALLOWED_CHARACTERS))
		it++;
	return std::string(line.begin(), it);
}

std::string get_value(std::string line)
{
	std::string::iterator it = line.begin();
	while (is_in_set(*it, PARAMETER_ALLOWED_CHARACTERS))
		it++;
	while (is_in_set(*it, CONF_DELIMITERS))
		it++;
	std::string value(it, line.end());
	code_minimize(value);
	return value;
}

void remove_comments(std::string& line)
{
	line = line.substr(0, line.find(COMMENT_CHAR));
}

bool valid_line(std::string line)
{
	if (line == "")
		return true;
	std::string::iterator it = line.begin();
	while (is_in_set(*it, PARAMETER_ALLOWED_CHARACTERS))
		it++;
	if (!is_in_set(*it, CONF_DELIMITERS))
		return false;
	while (is_in_set(*it, CONF_DELIMITERS))
		it++;
	return (it == line.end()) ? false : true;
}
