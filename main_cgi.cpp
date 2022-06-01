/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_cgi.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/26 14:21:22 by fde-capu          #+#    #+#             */
/*   Updated: 2022/06/01 16:32:47 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiWrapper.hpp"

DataFold load_configuration(int argc, char **argv, std::string conf_file)
{
	verbose(1) << "(wscgi) Loading configuration..." << std::endl;
	ArgVal validator(argc, argv);
	validator.load_conditions(const_cast<char*>(conf_file.c_str()));
	if (validator.fail())
		throw std::invalid_argument(BYE);
	return DataFold(validator.getConfig());
}

int main(int argc, char **argv)
{
	DataFold config = load_configuration(argc, argv, WSCGI_ARGVAL);

	CgiWrapper ws_cgi(argv[1], std::atoi(argv[2]));
	std::cout << ws_cgi << std::endl;
	return 0;
}
