/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strings.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/21 16:42:34 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/06 23:17:02 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.hpp"

bool die(std::string msg)
{
	std::cout << ALERT << ALERT_SEGUE << msg << std::endl;
	return false;
}

void alert(std::string msg)
{
	std::cout << ALERT << ALERT_SEGUE << msg << std::endl;
}

void alert(std::string msg, std::string &line)
{
	std::cout << ALERT << ALERT_SEGUE << msg << ALERT_SEGUE << line << std::endl;
}

void show_file(std::fstream &file)
{
	std::string line;
	std::cout << std::endl << file << " ::::::::" << std::endl;
	while (std::getline(file, line))
		std::cout << line << std::endl;
	std::cout << ":::::::::::::::::::::::" << std::endl << std::endl;
}
