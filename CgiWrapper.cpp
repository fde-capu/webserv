/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiWrapper.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/26 14:07:52 by fde-capu          #+#    #+#             */
/*   Updated: 2022/05/30 16:30:27 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiWrapper.hpp"
#include <iostream>

CgiWrapper::CgiWrapper(std::string u_executable, int u_port)
: executable(u_executable), port(u_port)
{
	verbose(1) << "Executable: " << executable << std::endl;
	verbose(1) << "Port: " << port << std::endl;

	struct pollfd ufds;

	try {
		listen_sock = WebServ::bind_socket_to_local(port);
	} catch (std::exception&e) {
		throw std::domain_error("(CgiWrap borrowed): " + std::string(e.what()));
	}
	ufds = pollfd();
	return ;
}

std::ostream & operator<< (std::ostream & o, CgiWrapper const & self)
{
	o << "::CgiWrapper::" << std::endl;
	static_cast<void>(self);
	return o;
}

CgiWrapper::~CgiWrapper()
{
	return ;
}
