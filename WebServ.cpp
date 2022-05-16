/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 14:24:28 by fde-capu          #+#    #+#             */
/*   Updated: 2022/05/16 22:56:31 by fde-capu         ###   ########.fr       */
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
		si.port.push_back(std::atoi(df_listen.val.c_str())); // htons?
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
	verbose(1) << "(webserv) Bound fd " << sfd << " to port " << u_port << "." << std::endl;
	return sfd;
}

void WebServ::hook_it()
{
	struct pollfd ufds;
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
}

struct pollfd WebServ::stdin_to_pollfd()
{
	struct pollfd ufds;

	ufds = pollfd();
	ufds.fd = 0;
	ufds.events = POLLIN;
	return ufds;
}

struct pollfd WebServ::make_pollin_fd(int newfd)
{
	struct pollfd ufds;

	ufds = pollfd();
	ufds.fd = newfd;
	ufds.events = POLLIN;
	return ufds;
}

void WebServ::exit_gracefully()
{
	verbose(1) << "Exit gracefully. Thanks!" << std::endl;
	return ;
}

// Set stdin.
// {%}
//  Get POLLIN, set pollin_fd.
//  If pollin_fd is any listen_sock of any instance,
//   generate newfd,
//   put newfd to poll,
//   close pollin_fd. // ?
//  If not, it sends data and  awaits response, then
//   receive,
//   analise and route (will generate response),
//   send response,
//   close pollin_fd.

int WebServ::catch_connection() const
{
	int TIME_OUT = 0; // 0 = non-blocking, -1 = blocking, N = cycle blocking ms
	int poll_count;
	DataFold event;

	while (1)
	{
		poll_count = poll(&poll_list[0], poll_list.size(), TIME_OUT);
		if (poll_count == -1)
			throw std::domain_error("(webserv) poll error.");
		for (size_t i = 0; i < poll_list.size(); i++)
			if (poll_list[i].revents & POLLIN)
				return poll_list[i].fd;
	}
}

void WebServ::light_up()
{
	int event;

	event = catch_connection();

	while (1)
	{
		// ...
	}
}

void WebServ::init()
{
	int TIME_OUT = 0; // 0 = non-blocking, -1 = blocking, N = cycle blocking ms
	struct sockaddr_storage remoteaddr;
	unsigned int addrlen;
	int poll_count;
	int newfd;

	hook_it();
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
				verbose(1) << "(webserv) Got POLLIN from (i = " << i << "), fd " << poll_list[i].fd << "." << std::endl;

				for (size_t j = 0; j < instance[0].listen_sock.size(); j++)
				{
					verbose(1) << "j " << j << " < " << instance[0].listen_sock.size() << std::endl;
					if (poll_list[i].fd == instance[0].listen_sock[j])
					{
						verbose(1) << "(" << poll_list[i].fd << " == " << instance[0].listen_sock[j] << ")" << std::endl;
						addrlen = sizeof remoteaddr;
						newfd = accept(instance[0].listen_sock[j], (struct sockaddr *)&remoteaddr, &addrlen);
						if (newfd == -1)
							throw std::domain_error("(webserv) Unacceptable connection.");
						else
						{
							poll_list.push_back(make_pollin_fd(newfd));
							verbose(1) << "(webserv) New connection on fd (" << poll_list[i].fd << ")->" << newfd << std::endl;
						}
					}
					else
					{
						verbose(1) << "(webserv) Constructing buffer for fd " << poll_list[i].fd << "." << std::endl;
						CircularBuffer buffer(poll_list[i].fd);
						buffer.receive_until_eof();
//						if (!buffer.ended())
//						{
							verbose(1) << "(webserv) Got data from " << poll_list[i].fd << "." << std::endl;
							verbose(1) << "-->" << buffer.output << "<--" << std::endl;
//						}
//						else
//						{
							close(poll_list[i].fd);
							poll_list.erase(poll_list.begin() + i);
							if (poll_list[i].fd == 0) // stdin
								return exit_gracefully();
//							verbose(1) << "(webserv) fd " << poll_list[i].fd << " hung up." << std::endl;
//						}
					}
				}
			}
		}
	}
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
