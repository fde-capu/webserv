/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiWrapper.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/26 14:07:52 by fde-capu          #+#    #+#             */
/*   Updated: 2022/06/01 15:00:12 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiWrapper.hpp"
#include <iostream>

CgiWrapper::CgiWrapper(std::string u_executable, int u_port)
: executable(u_executable), port(u_port), fail(true)
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
	while (!event)
	{
		poll_count = poll(&poll_list[0], poll_list.size(), TIME_OUT);
		if (poll_count == -1)
			throw std::domain_error("(CgiWrapper) Poll crashed.");
		for (size_t i = 0; i < poll_list.size(); i++)
		{
			if (poll_list[i].revents & POLLIN)
			{
				std::cout << "e: " << event << ", pl[i]fd: " << poll_list[i].fd << std::endl;
				event = poll_list[i].fd;
				break ;
			}
		}
	}

	// Answer the phone.
	newfd = accept(event, (struct sockaddr *)&remoteaddr, &addrlen);
	if (newfd == -1)
		throw std::domain_error("(CgiWrapper) Could not accept call.");
	ufds.fd = newfd;
	poll_list.push_back(ufds);
	verbose(1) << "(CgiWrapper) Received call (" << event << ")->" << newfd << "." << std::endl;

	// Read data.
	raw_data = CircularBuffer(newfd);
	std::cout << "[RAW>>" << raw_data << "<<RAW]" << std::endl;

	// Parse content.
	ws_server_instance si;
	si.in_header = WebServ::get_header(raw_data);
	if (!si.in_header.is_valid)
	{
		remove_from_poll(newfd);
		return ;
	}
	si.in_body = WebServ::get_body(raw_data);

	// Make up some response.
	ws_reply_instance respond(si, executable);
	if (send(newfd, respond.encapsulate().c_str(), respond.package_length, 0) == -1)
		throw std::domain_error("(CgiWrapper) Could not respond.");

	remove_from_poll(newfd);
	fail = false;
}

void CgiWrapper::remove_from_poll(int fd)
{
	for (size_t i = 0; i < poll_list.size(); i++)
	{
		if (poll_list[i].fd == fd)
		{
			std::vector<struct pollfd>::iterator position(&poll_list[i]);
			poll_list.erase(position);
			return ;
		}
	}
	throw std::domain_error("(CgiWrapper) Can not remove fd from poll.");
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
