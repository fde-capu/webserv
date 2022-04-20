/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 21:07:26 by fde-capu          #+#    #+#             */
/*   Updated: 2022/04/20 14:19:01 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.hpp"

bool validate_args(int argc, char **argv)
{
//	ArgVal av = ArgVal(argc, argv, const_cast<char *>(WS_CONFIG_SETUP));
	ArgVal av = ArgVal(argc, argv, WS_CONFIG_SETUP);
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

	verbose(1) << "Configuration valid." << std::endl;
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
