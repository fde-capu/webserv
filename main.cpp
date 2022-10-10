/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 21:07:26 by fde-capu          #+#    #+#             */
/*   Updated: 2022/10/10 23:31:54 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
DataFold g_config;

DataFold load_configuration(int argc, char **argv, std::string conf_file)
{
	verbose(1) << "Loading configuration..." << std::endl;
	char* args[2];
	args[0] = argv[0];
	if (argc == 1)
	{
		args[1] = const_cast<char *>(WS_DEFAULT_CONFIG);
		argc = 2;
	}
	else
		args[1] = argv[1];
	ArgVal validator(argc, args, conf_file.c_str());
	if (validator.fail())
		throw std::invalid_argument(BYE);
	return DataFold(validator.getConfig());
}

int main(int argc, char **argv)
{
	try
	{
		TemplateError load(argv[0], "template_default.conf");
		g_config = load_configuration(argc, argv, WS_ARGVAL);
		WebServ webserv(g_config);
		webserv.init();
	} catch(std::exception&) { return 1; };
	return 0;
}
