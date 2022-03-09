/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 21:07:26 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/09 14:18:49 by fde-capu         ###   ########.fr       */
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

//	std::cout << file_conf << std::endl;
//	std::cout << file_conf["server_name"] << std::endl;
//	std::cout << ">" << file_conf["server"] << "<" << std::endl;
//
//	std::string servn = file_conf["server_name"];
//	std::string serv = file_conf["server"];
//	std::cout << "--" << servn << "--" << std::endl;
//	std::cout << "--" << serv << "--" << std::endl;

//	int test_int = file_conf["client_max_body_size"];
//	int test_int = file_conf.getInt("test_port");
	int test_int = file_conf.get<int>("test_port");
	std::cout << "int: " << test_int << std::endl;

	std::vector<int> test_single_vec = file_conf.get<std::vector<int> >("test_port");
	std::cout << "single vec: " << test_single_vec[0] << std::endl;

//	int fail_int = file_conf.get<int>("bye_message");
//	std::cout << "fail: " << fail_int << std::endl;

//	std::string string_int = file_conf.get<std::string>("test_port");
//	std::cout << "string int: " << string_int << std::endl;

	int arr_int = file_conf.get<std::vector<int> >("test_int_array")[1];
	std::cout << "int array: " << arr_int << std::endl;

//	std::string servn = file_conf.get<std::string>("server_name");
//	std::string serv = file_conf["server"];
//	std::cout << "--" << servn << "--" << std::endl;

//	std::string serv = file_conf.get<std::string>("server");
//	std::cout << "--" << serv << "--" << std::endl;

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
