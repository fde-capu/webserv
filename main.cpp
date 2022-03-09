/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 21:07:26 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/09 19:58:32 by fde-capu         ###   ########.fr       */
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

bool read_conf(maps & conf, const char *file)
{
	FileString file_conf(file);
	if (file_conf.fail())
		return die(ERR_INVALID_FILE);

	(void)conf;
	return true;
}

bool essential_configuration(maps & conf)
{
	(void)conf;
	/// To do. Has to have at least one server
	// with at least "listen"
	return true;
}

int main(int argc, char **argv)
{
	FileString conf;

//	if (!validate_args(argc, argv))
//		return !die(BYE);
	conf.load(DEFAULT_CONFIG_FILE);
	
//	if (!read_conf(conf, DEFAULT_CONFIG_FILE) || !read_conf2(conf, argv[1]) ||
//		!essential_configuration(conf))
//		return !die(BYE);

	std::string name = conf.get<std::string>("server_name");
	std::cout << "name: " << name << std::endl;

	if (conf.success())
		alert(CONFIG_OK);
	if (conf.fail())
		return !die(BYE);

	(void)argc;
	(void)argv;
	return 0;
}
