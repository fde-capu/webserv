/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_cgi.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/26 14:21:22 by fde-capu          #+#    #+#             */
/*   Updated: 2022/05/27 13:25:21 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.hpp"

int main(int argc, char **argv)
{
	if (ArgVal(argc, 3, argv, "_ filename size_t").fail())
		throw std::invalid_argument(BYE);

	CgiWrapper ws_cgi(argv[1], std::atoi(argv[2]));
	std::cout << ws_cgi << std::endl;
	return 0;
}
