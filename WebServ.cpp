/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 14:24:28 by fde-capu          #+#    #+#             */
/*   Updated: 2022/05/02 03:16:34 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
#include <iostream>

ws_serv_instance WebServ::dftosi(DataFold df)
{
	ws_serv_instance si;
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
//			bind(	reinterpret_cast<struct sockaddr *>(&si),
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

void WebServ::listen_on(int listen_sock)
{
	(void)listen_sock;
//	for (size_t i = 0; i < port_count(); i++)
//		listen(instance[i].server_socket, 1);
//	if (listen(listen_sock, SOMAXCONN) != 0)
//		throw std::domain_error("(webserv) Listening problem.");
}

int WebServ::bind_socket_to_local(int u_port)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd, addrinfo_out;

	hints = addrinfo();
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
//	hints.ai_protocol = 0;
//	hints.ai_canonname = NULL;
//	hints.ai_addr = NULL;
//	hints.ai_next = NULL;

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
		throw std::domain_error("(webserv) Could not find a socket.");
	freeaddrinfo(result);
	verbose(1) << "Bound fd " << sfd << " to port " << u_port << "." << std::endl;
	return sfd;
}

void WebServ::init()
{
//	bind_ports();
//	listen_on();
//	hook_it();
//	listen_on(listen_sock);
//	accept_connection();

	int READ_SIZE = 10;
	int TIME_OUT = -1;
	size_t bytes_read;
	char read_buffer[READ_SIZE + 1];
	struct epoll_event events[MAX_EVENTS];
	int s;
	int listen_sock;
	struct epoll_event ev;
	struct addrinfo *result;
	int nfds;
	int lit = 1;
	int conn_sock;
//	bool test = true;
//	char test[i] = '4';

	listen_sock = bind_socket_to_local(3491);

//	int epollfd = epoll_create(1); // Argument must only be non-zero for historical reasons.
	int epollfd = epoll_create1(0);
	if (epollfd == -1)
		throw std::domain_error("(webserv) epoll_create1 blew up.");
	verbose(1) << "Poll socket fd: " << epollfd << "." << std::endl;

	ev.events = EPOLLIN;
	ev.data.fd = listen_sock;

//	s = = epoll_ctl(itoa(epollfd).c_str(), EPOLL_CTL_ADD, listen_sock, &ev);
	s = epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev);
	if (s == -1)
	{
		close(epollfd);
		throw std::domain_error("(webserv) Could not add socket to poll.");
	}

//	s = listen(epollfd, SOMAXCONN);
//	getaddrinfo(itoa(epollfd).c_str(), NULL, NULL, &result);
//	std::cout << "ai_flags " << result->ai_flags << std::endl;
//	if (s == -1)
//		throw std::domain_error("(webserv) Listening did not perform: " + std::string(gai_strerror(s)));

//	while (lit++ < 20)
	while (lit)
	{
//		std::cout << "_" << std::endl;
		nfds = epoll_wait(epollfd, events, MAX_EVENTS, TIME_OUT);
		if (nfds == -1)
			throw std::domain_error("(webserv) epoll could not wait.");
		if (nfds)
			std::cout << nfds << " events!" << std::endl;
		for (int n = 0; n < nfds; n++)
		{
			if (events[n].data.fd == listen_sock)
			{
				conn_sock = accept(listen_sock,
					(struct sockaddr *) &addr, &addrlen);
				if (conn_sock == -1)
					throw std::domain_error("(webserv) Some socket is unacceptable.");
				setnonbloacking(conn_sock);
				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = conn_sock;
				if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1)
					throw std::domain_error("(webserv) epoll_ctl failed to dup accepted connection.");
			}
			else
			{
				int evfd = events[n].data.fd;
				bytes_read = read(evfd, read_buffer, READ_SIZE);
				read_buffer[bytes_read] = '\0';
				std::cout << "GOT " << read_buffer << std::endl;
//				do_use_fd(events[n].data.fd); // until read or write yells EAGAIN
			}
		}
	}
	
	if (close(epollfd))
		throw std::domain_error("(webserv) epoll socket was closed, but something weird happened.");
	
//	epoll_wait(epollfd, &ev, 64, 0);
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
