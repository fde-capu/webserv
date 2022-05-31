/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiWrapper.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/26 14:07:52 by fde-capu          #+#    #+#             */
/*   Updated: 2022/05/31 11:59:05 by fde-capu         ###   ########.fr       */
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
	int poll_count;
	int TIME_OUT = 0; // 0 = non-blocking, -1 = blocking, N = cycle blocking ms
	int event;
	struct sockaddr_storage remoteaddr;
	unsigned int addrlen = sizeof remoteaddr;
	int newfd;
	std::string raw_data;

	// Bind
	try {
		listen_sock = WebServ::bind_socket_to_local(port);
	} catch (std::exception&e) {
		throw std::domain_error("(CgiWrapper borrowed): " + std::string(e.what()));
	}

	// Listen
	ufds = pollfd();
	ufds.fd = listen_sock;
	ufds.events = POLLIN;
	poll_list.push_back(ufds);
	// fd_to_instance[] ?
	if (listen(listen_sock, SOMAXCONN) != 0)
		throw std::domain_error("(CgiWrapper) Listening went wrong.");
	verbose(1) << "^> (CgiWrapper) I'm listening." << std::endl;

	// Catch connection.
	event = 0;
	while (1)
	{
		poll_count = poll(&poll_list[0], poll_list.size(), TIME_OUT);
		if (poll_count == -1)
			throw std::domain_error("(CgiWrapper) Poll crashed.");
		for (size_t i = 0; i < poll_list.size(); i++)
		{
			if (poll_list[i].revents & POLLIN)
			{
				event = poll_list[i].fd;
				break ;
			}
		}
	}

	newfd = accept(event, (struct sockaddr *)&remoteaddr, &addrlen);
	if (newfd == -1)
		throw std::domain_error("(CgiWrapper) Could not accept call.");
	verbose(1) << "(CgiWrapper) Received call (" << event << ")->" << newfd << "." << std::endl;

	raw_data = CircularBuffer(newfd).reof_out();
	std::cout << "RAW>>" << raw_data << "<<" << std::endl;

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
