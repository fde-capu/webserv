/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 21:07:26 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/12 21:36:32 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.hpp"

bool validate_args(int argc, char **argv)
{
	char argconf[] = "argval.conf";
	ArgVal av = ArgVal(argc, argv, argconf);
//	av.load_conditions("argval.conf");
//	std::cout << "fail: " << av.fail() << std::endl;

//	if (argc > 2)
//		return die(ERR_INVALID_ARGS);

//	return true || false;
	return true;
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
	
//	if (conf.success())
//		alert(CONFIG_OK);
//	if (conf.fail())
//		return !die(BYE);

	(void)argc;
	(void)argv;
	return 0;
}
