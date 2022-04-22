/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 21:07:26 by fde-capu          #+#    #+#             */
/*   Updated: 2022/04/22 13:11:17 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.hpp"

ArgVal load_config_files(int argc, char **argv)
{
	char* args[2] = {argv[0], argv[1]};
	if (argc == 1)
	{
		args[1] = const_cast<char *>(WS_DEFAULT_CONFIG);
		argc = 2;
	}
	return ArgVal(argc, args, WS_CONFIG_SETUP);
}

DataFold load_configuration(int argc, char **argv)
{
	ArgVal validator = load_config_files(argc, argv);
	if (validator.fail())
		throw std::invalid_argument(BYE);
	verbose(1) << "Configuration valid." << std::endl;
	return DataFold(validator.getConfig());
}

int main(int argc, char **argv)
{
	DataFold config = load_configuration(argc, argv);
	std::string server_name = config.get("server_name");
	std::cout << "CONFIG" << std::endl << server_name;


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
