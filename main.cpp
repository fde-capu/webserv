/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 21:07:26 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/28 21:07:32 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.hpp"

bool validate_args(int argc, char **argv)
{
	char argconf[] = WS_CONFIG_SETUP;
	ArgVal av = ArgVal(argc, argv, argconf);
	return av.success();
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
