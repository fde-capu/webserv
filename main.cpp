/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 21:07:26 by fde-capu          #+#    #+#             */
/*   Updated: 2022/02/18 20:50:11 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.hpp"
#include "defines.hpp"
#include <string>
#include <iostream>
#include <cstdlib>
#include <fstream>

void die(std::string msg)
{
	std::cout << ERR << msg << std::endl;
	exit (1);
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

std::string code_minimize(std::string str)
{
	str = trim_lr_set(str, TRIM_CHARACTERS);
	return str;
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
	line = line.substr(0, line.find(CONF_DELIMITER));
	std::string parameter("");
	return parameter;
}

bool valid_line(std::string line)
{
	line = line.substr(0, line.find(COMMENT_CHAR));
	line = code_minimize(line);
	if (line == "")
		return true;
	if (line.find(CONF_DELIMITER) == std::string::npos)
		return false;
	return true;
}

maps read_conf(std::fstream &file)
{
	maps conf;
	std::string line;
	std::string split_a, split_b;
	while (std::getline(file, line))
	{
		if (!valid_line(line))
			die(ERR_INVALID_PARAM + std::string(ERR_SEGUE) + line);
		split_a = get_parameter(line);
		split_b = split_a;
		conf[split_a] = split_b;
		std::cout << "[" << split_a << "] = " << split_b << ";" << std::endl;
	}
	return conf;
}

maps validate_args(int argc, char **argv)
{
	if (argc != 2)
		die(ERR_INVALID_ARGS);
	std::fstream conf;
	conf.open(static_cast<const char *>(argv[1]), std::ios::in);
	if (!conf)
		die(ERR_INVALID_FILE);
	show_file(conf);
	conf.close();
	conf.open(static_cast<const char *>(argv[1]), std::ios::in);
	maps conf_map = read_conf(conf);
	conf.close();
	return conf_map;
}

int main (int argc, char **argv)
{
	maps g_conf = validate_args(argc, argv);
	return 0;
}
