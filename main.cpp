/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 21:07:26 by fde-capu          #+#    #+#             */
/*   Updated: 2022/04/20 14:39:26 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.hpp"

bool validate_args(int argc, char **argv)
{
	return ArgVal(argc, argv, WS_CONFIG_SETUP).success();
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
