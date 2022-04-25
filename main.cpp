/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 21:07:26 by fde-capu          #+#    #+#             */
/*   Updated: 2022/04/25 21:48:20 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.hpp"

DataFold load_configuration(int argc, char **argv)
{
	verbose(1) << "Validating configuration..." << std::endl;
	char* args[2] = {argv[0], argv[1]};
	if (argc == 1)
	{
		args[1] = const_cast<char *>(WS_DEFAULT_CONFIG);
		argc = 2;
	}
	ArgVal validator(argc, args, WS_CONFIG_SETUP);
	if (validator.fail())
		throw std::invalid_argument(BYE);
	verbose(1) << "Configuration valid." << std::endl;
	return DataFold(validator.getConfig());
}

int main(int argc, char **argv)
{
	DataFold config = load_configuration(argc, argv);
	WebServ webserv(config);
	webserv.init();
	return 0;
}
