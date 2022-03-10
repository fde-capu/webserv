/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 21:07:26 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/10 18:36:03 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.hpp"

bool validate_args(int argc, char **argv)
{
	ArgVal av = ArgVal(argc, argv);
	std::cout << av << std::endl;
	std::cout << "fail: " << av.fail() << std::endl;

	if (argc > 2)
		return die(ERR_INVALID_ARGS);

	return false;
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

	if (!validate_args(argc, argv))
		return !die(BYE);

	conf.load(DEFAULT_CONFIG_FILE);
	conf.load("test.conf");
	
	if (conf.success())
		alert(CONFIG_OK);
	if (conf.fail())
		return !die(BYE);

	(void)argc;
	(void)argv;
	return 0;
}
