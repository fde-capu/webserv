/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 14:24:28 by fde-capu          #+#    #+#             */
/*   Updated: 2022/04/29 13:54:55 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
#include <iostream>

server_instance WebServ::dftosi(DataFold df)
{
	server_instance si;
	si.current_http_header = "HTTP/1.1 200 OK\nConnection: close\nContent-Length: 13\n\n";
	si.current_http_body = "Hello, world!";
	si.server_socket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in endpoint;
	DataFold df_listen(df.get("listen"));
	while (df_listen.loop())
	{
		endpoint.sin_family = AF_INET;
		endpoint.sin_addr.s_addr = INADDR_ANY;
		endpoint.sin_port = htons(std::atoi(df_listen.val.c_str()));
		si.server_address.push_back(endpoint);
	}
	si.attribute = df;
	return si;
}

void WebServ::bind_ports()
{
	struct sockaddr_in si;

	for (size_t i = 0; i < port_count(); i++)
	{
		for (size_t j = 0; j < instance[i].server_address.size(); j++)
		{
			si = instance[i].server_address[j];

//			Safe for bind?:
//					reinterpret_cast<struct sockaddr *>(&si),
			bind(	instance[i].server_socket,
					(struct sockaddr *)&si,
					sizeof(si)
				);
		}
	}
}

size_t WebServ::port_count()
{ return instance.size(); }

void WebServ::hook_it()
{
//	struct sockaddr_in *communication_layer;
//	int client_socket;
//	struct addrinfo hints;
//	struct addrinfo *result;
//	struct addrinfo *info;
//	int sign;
//	char netstr[INET6_ADDRSTRLEN];
//	struct sockaddr_in *port_in;
//	struct sockaddr_in6 *port_in6;
//	std::string content;
//
//	verbose(0) << "Server is (will be) up." << std::endl;
//
//	while (1)
//	{
//		for(size_t i = 0; i < instance.size(); i++)
//		{
////			i = 0;
//			//			communication_layer = calloc(1, sizeof(struct sockaddr_in));
//			communication_layer = new sockaddr_in();
//			client_socket = accept(
//					instance[i].server_socket,
//					(struct sockaddr*)&communication_layer,
//					(socklen_t *)&communication_layer->sin_addr.s_addr
//					);
//			hints = addrinfo();
//			//			memset(&hints, 0, sizeof(struct addrinfo));
//			hints.ai_family = AF_UNSPEC;
//			hints.ai_socktype = SOCK_DGRAM;
//			hints.ai_flags = AI_PASSIVE;
//			hints.ai_protocol = 0;
//			hints.ai_canonname = NULL;
//			hints.ai_addr = NULL;
//			hints.ai_next = NULL;
//			sign = getaddrinfo(NULL, itoa(instance[i].server_socket).c_str(), &hints, &result);
//			if (sign != 0)
//			{
//				verbose(0) << ALERT << " " << ERROR << ERR_FAILED_ADDRINFO << std::endl;
//				continue ;
//			}
//			for(info = result; info != NULL; info = info->ai_next)
//			{
//				if (info->ai_addr->sa_family == AF_INET)
//				{
//					port_in = (struct sockaddr_in *)info->ai_addr;
//					verbose(0) << "Connection from: " << inet_ntop(AF_INET, &port_in->sin_addr, netstr, sizeof(netstr)) << std::endl;
//				}
//				else if (info->ai_addr->sa_family == AF_INET6)
//				{
//					port_in6 = (struct sockaddr_in6 *)info->ai_addr;
//					verbose(0) << "Connection from: " << inet_ntop(AF_INET6, &port_in6->sin6_addr, netstr, sizeof(netstr)) << std::endl;
//				}
//			}
//
//			verbose(0) << ALERT << " Family: " << communication_layer->sin_family << std::endl;
//			verbose(0) << ALERT << " Port: " << ntohs(communication_layer->sin_port) << std::endl;
//			verbose(0) << ALERT << " in_addr: " << communication_layer->sin_addr.s_addr << std::endl;
//
//			content = instance[i].current_http_header + instance[i].current_http_body;
//			send(
//				client_socket,
//				content.c_str(),
//				content.length(),
//				0
//			);
//
//			verbose(0) << ALERT << " sent (" << content.length() << "): ```" << content << "```" << std::endl;
//
//			close(client_socket);
//			delete communication_layer;
//		}
//		//////////
////		std::cout << "!!!!" << std::endl;
////		break ;
//	}
}

void WebServ::listen_on(int sockfd)
{
	(void)sockfd;
//	for (size_t i = 0; i < port_count(); i++)
//		listen(instance[i].server_socket, 1);
//	if (listen(sockfd, SOMAXCONN) != 0)
//		throw std::domain_error("(webserv) Listening problem.");
}

int WebServ::bind_socket_to_local(int u_port)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd, addrinfo_out;

	hints = addrinfo();
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = 0;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	addrinfo_out = getaddrinfo(NULL, itoa(u_port).c_str(), &hints, &result);
	if (addrinfo_out != 0)
		throw std::domain_error("(webserv) getaddrinfo failed: " + std::string(gai_strerror(addrinfo_out)));
	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
			continue;
		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
			break;
		close(sfd);
	}
	if (rp == NULL)
		throw std::domain_error("(webserv) Could not connect.");
	freeaddrinfo(result);
	verbose(1) << "Got fd " << sfd << " bound to port " << u_port << "." << std::endl;
	return sfd;
}

void WebServ::init()
{
//	bind_ports();
//	listen_on();
//	hook_it();
//	listen_on(sockfd);
//	accept_connection();

	int sockfd = bind_socket_to_local(3491);
	int poll_sock = epoll_create(1); // Argument must only be non-zero for historical reasons.
	verbose(1) << "pool socket: " << poll_sock << "." << std::endl;

	int READ_SIZE = 10;
	int TIME_OUT = -1;
	size_t bytes_read;
	char read_buffer[READ_SIZE + 1];

	struct epoll_event event;
	struct epoll_event event_list[MAX_EVENTS];
	event.events = EPOLLIN;
	event.data.fd = sockfd;

	if (epoll_ctl(poll_sock, EPOLL_CTL_ADD, sockfd, &event) == -1)
	{
		close(poll_sock);
		throw std::domain_error("(webserv) Could not add socket to poll.");
	}

	int event_count;
	int lit = 1;
	while (lit++ < 20)
	{
		std::cout << "_";
		event_count = epoll_wait(poll_sock, event_list, MAX_EVENTS, TIME_OUT);
		std::cout << "e" << event_count << " ";
		for (int i = 0; i < event_count; i++)
		{
			std::cout << "Event from fd " << event_list[i].data.fd << ":";
			bytes_read = read(event_list[i].data.fd, read_buffer, READ_SIZE);
			read_buffer[bytes_read] = '\0';
			std::cout << read_buffer << std::endl;
		}
	}
	
	if (close(poll_sock))
		throw std::domain_error("(webserv) epoll socket could not be closed.");
	
//	epoll_wait(poll_sock, &event, 64, 0);
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

std::vector<server_instance> WebServ::getInstance() const
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
