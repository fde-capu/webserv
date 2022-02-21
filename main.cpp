/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 21:07:26 by fde-capu          #+#    #+#             */
/*   Updated: 2022/02/21 17:04:49 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.hpp"

bool validate_args(int argc, char **argv)
{
	if (argc != 2)
		return die(ERR_INVALID_ARGS);
	std::fstream conf;
	conf.open(static_cast<const char *>(argv[1]), std::ios::in);
	if (!conf)
		return die(ERR_INVALID_FILE);
	conf.close();
	return true;
}

bool read_conf(maps & conf, const char *file)
{
	std::fstream conf_stream;

	conf_stream.open(file, std::ios::in);
	std::string line, parameter, value;

	while (std::getline(conf_stream, line))
	{
		remove_comments(line);
		code_minimize(line);
		if (!valid_line(line))
		{
			alert(ERR_INVALID_PARAM, line);
			conf.clear();
			return false;
		}
		if (line == "")
			continue ;
		parameter = get_parameter(line);
		value = get_value(line);
		conf[parameter] = value;
		if (VERBOSE)
			std::cout << "[" << parameter << "] = [" << value << "];" << std::endl;
	}
	conf_stream.close();
	return true;
}

bool essential_configuration(maps & conf)
{
}

int main(int argc, char **argv)
{
	maps conf;

	if (!validate_args(argc, argv))
		return !die(ERR_INVALID_ARGS);
	if (!read_conf(conf, DEFAULT_CONFIG_FILE) || !read_conf(conf, argv[1]) ||
		!essential_configuration(conf))
		return 1;
	alert(CONFIG_OK);
	return 0;
}
