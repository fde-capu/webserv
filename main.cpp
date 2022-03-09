/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 21:07:26 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/09 14:54:23 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.hpp"

bool validate_args(int argc, char **argv)
{
	if (argc > 2)
		return die(ERR_INVALID_ARGS);
	return true;
	(void)argv;
}

bool read_conf2(maps & conf, const char *file)
{
	FileString file_conf(file);
	if (file_conf.fail())
		return die(ERR_INVALID_FILE);
	file_conf.parse();

//	int sub_int = file_conf.get<int>("server");
//	std::cout << "sub_int: " << sub_int << std::endl;
	// Ok error server is array.

//	std::vector<int> sub_vint = file_conf.get<std::vector<int> >("server");
//	std::cout << "sub_vint: " << sub_vint[0] << std::endl;
	// Ok error server is not a number.

	std::string sub_str = file_conf.get<std::string>("double_message");
	std::cout << "sub_str: " << sub_str << std::endl;
	// Ok error server is array when server is array.
	// Ok error when server is na object.

	(void)conf;
	return true;
}

//bool read_conf(maps & conf, const char *file)
//{
//	static bool
//	std::fstream conf_stream;
//
//	conf_stream.open(file, std::ios::in);
//	std::string line, parameter, value;
//
//	while (std::getline(conf_stream, line))
//	{
//		remove_comments(line);
//		code_minimize(line);
//		if (!valid_line(line))
//		{
//			alert(ERR_INVALID_PARAM, line);
//			conf.clear();
//			return false;
//		}
//		if (line == "")
//			continue ;
//		parameter = get_parameter(line);
//		value = get_value(line);
//		conf[parameter] = value;
//		if (VERBOSE)
//			std::cout << "[" << parameter << "] = [" << value << "];" << std::endl;
//	}
//	conf_stream.close();
//	return true;
//}

bool essential_configuration(maps & conf)
{
	(void)conf;
	/// To do. Has to have at least one server
	// with at least "listen"
	return true;
}

int main(int argc, char **argv)
{
	maps conf;

	if (!validate_args(argc, argv))
		return !die(BYE);
//	if (!read_conf2(conf, DEFAULT_CONFIG_FILE) || !read_conf2(conf, argv[1]) ||
	if (!read_conf2(conf, argv[1]) ||
		!essential_configuration(conf))
		return 1;
	alert(CONFIG_OK);
	return 0;
}
