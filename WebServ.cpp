/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 14:24:28 by fde-capu          #+#    #+#             */
/*   Updated: 2022/05/12 14:28:49 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
#include <iostream>

ws_serv_instance WebServ::dftosi(DataFold df)
{
	ws_serv_instance si;
	si.current_http_header = "HTTP/1.1 200 OK\nConnection: close\nContent-Length: 13\n\n";
	si.current_http_body = "Hello, world!";
	DataFold df_listen(df.get("listen"));
	while (df_listen.loop())
	{
		si.port.push_back(std::atoi(df_listen.val.c_str())); // htons?
	}
	return si;
}

int WebServ::bind_socket_to_local(int u_port)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd, s;
	int yes = 1;

	hints = addrinfo();
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	s = getaddrinfo(NULL, itoa(u_port).c_str(), &hints, &result);
	if (s != 0)
		throw std::domain_error("(webserv) getaddrinfo failed: " + std::string(gai_strerror(s)));

	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
			throw std::domain_error("(webserv) Socket creation failed.");
		if (fcntl(sfd, F_SETFD, O_NONBLOCK) == -1)
			throw std::domain_error("(webserv) Failed to set non-blocking flag.");
		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
			throw std::domain_error("(webserv) Can not reuse socket.");
		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
			break;
		close(sfd);
	}
	if (rp == NULL)
		throw std::domain_error("(webserv) Socket locked.");

	freeaddrinfo(result);
	verbose(1) << "Bound fd " << sfd << " to port " << u_port << "." << std::endl;
	return sfd;
}

std::vector<struct pollfd> WebServ::hook_it()
{
	std::vector<struct pollfd> poll_list;
	struct pollfd ufds;
//	struct sockaddr_in si;

//	for (size_t i = 0; i < port_count(); i++)
//	{
		for (size_t j = 0; j < instance[0].port.size(); j++)
		{

	instance[0].listen_sock.push_back(bind_socket_to_local(instance[0].port[j]));
	ufds = pollfd();
	ufds.fd = instance[0].listen_sock[j];
	ufds.events = POLLIN;
	poll_list.push_back(ufds);
	if (listen(instance[0].listen_sock[j], SOMAXCONN) != 0)
		throw std::domain_error("(webserv) Listening problem.");

		}
//	}
	return poll_list;
}

struct pollfd WebServ::stdin_to_pollfd()
{
	struct pollfd ufds;

	ufds = pollfd();
	ufds.fd = 0;
	ufds.events = POLLIN;
	return ufds;
}

void WebServ::init()
{
	int TIME_OUT = 0; // 0 = non-blocking, -1 = blocking, N = cycle blocking ms
	std::vector<struct pollfd> poll_list;
	struct sockaddr_storage remoteaddr;
	unsigned int addrlen;
	int poll_count;
	int newfd;
	std::string new_line;
	struct pollfd ufds;

	poll_list = hook_it();
	poll_list.push_back(stdin_to_pollfd());

	while (1)
	{
		poll_count = poll(&poll_list[0], poll_list.size(), TIME_OUT);
		if (poll_count == -1)
			throw std::domain_error("(webserv) poll error.");
		for (size_t i = 0; i < poll_list.size(); i++)
		{
			if (poll_list[i].revents & POLLIN)
			{
				for (size_t j = 0; j < instance[0].listen_sock.size(); j++)
				{
					if (poll_list[i].fd == instance[0].listen_sock[j])
					{
						addrlen = sizeof remoteaddr;
						newfd = accept(instance[0].listen_sock[j], (struct sockaddr *)&remoteaddr, &addrlen);
						if (newfd == -1)
							throw std::domain_error("(webserv) Unacceptable connection.");
						else
						{
							ufds = pollfd();
							ufds.fd = newfd;
							ufds.events = POLLIN;
							poll_list.push_back(ufds);
							verbose(1) << "(webserv) New connection on fd (" << poll_list[i].fd << ")-> " << newfd << std::endl;
						}
					}
					else
					{
						new_line = gnl(poll_list[i].fd);
						if (new_line == "")
						{
							verbose(1) << "(webserv) fd " << poll_list[i].fd << " hung up." << std::endl;
							close(poll_list[i].fd);
							poll_list.erase(poll_list.begin() + i);
							if (poll_list[i].fd == 0) // stdin
								exit(0);
						}
						else
						{
							verbose(1) << "(webser) Got data from " << poll_list[i].fd << "." << std::endl;
							verbose(1) << new_line << std::endl;
						}
					}
				}
			}
		}
	}
}

std::string WebServ::gnl(int fd)
{
	CircularBuffer buffer(fd);
	return buffer.output;
}

WebServ::WebServ(DataFold& u_config)
: config(u_config), server(config.filter("server"))
{
	while (server.loop())
		instance.push_back(dftosi(server.val));
	return ;
}

WebServ::WebServ(WebServ const & src)
{
	*this = src;
	return ;
}

DataFold WebServ::getConfig() const
{ return config; }

DataFold WebServ::getServer() const
{ return server; }

std::vector<ws_serv_instance> WebServ::getInstance() const
{ return instance; }

WebServ & WebServ::operator= (WebServ const & rhs)
{
	if (this != &rhs)
	{
		config = rhs.getConfig();
		server = rhs.getServer();
		instance = rhs.getInstance();
	}
	return *this;
}

std::ostream & operator<< (std::ostream & o, WebServ const & self)
{
	o << "::WebServ::" << std::endl;
	(void)self;
	return o;
}

WebServ::~WebServ()
{
	return ;
}
