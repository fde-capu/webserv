/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 21:07:26 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/11 13:46:00 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.hpp"

bool validate_args(int argc, char **argv)
{
	ArgVal av = ArgVal(argc, argv);
	std::cout << "fail: " << av.fail() << std::endl;

	if (argc > 2)
		return die(ERR_INVALID_ARGS);

	return false;
	(void)argv;
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

//	conf.load(DEFAULT_CONFIG_FILE);
//	conf.load("test.conf");
	
	if (conf.success())
		alert(CONFIG_OK);
	if (conf.fail())
		return !die(BYE);

	(void)argc;
	(void)argv;
	return 0;
}
