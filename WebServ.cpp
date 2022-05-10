/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 14:24:28 by fde-capu          #+#    #+#             */
/*   Updated: 2022/05/10 14:11:06 by fde-capu         ###   ########.fr       */
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

void WebServ::init()
{
//	bind_ports();
//	listen_on();
//	hook_it();
//	listen_on(listen_sock);
//	accept_connection();

	int TIME_OUT = -1;
	int listen_sock;
	struct pollfd ufds;
	std::vector<struct pollfd> poll_list;
	struct sockaddr_storage remoteaddr;
	unsigned int addrlen;
	int poll_count;
	int newfd;
	std::string new_line;

	listen_sock = bind_socket_to_local(3491);

	ufds = pollfd();
	ufds.fd = listen_sock;
	ufds.events = POLLIN;
	poll_list.push_back(ufds);

	if (listen(listen_sock, SOMAXCONN) != 0)
		throw std::domain_error("(webserv) Listening problem.");

	while (1)
	{
		poll_count = poll(&poll_list[0], poll_list.size(), TIME_OUT);
		if (poll_count == -1)
			throw std::domain_error("(webserv) poll error.");
		for (size_t i = 0; i < poll_list.size(); i++)
		{
			if (poll_list[i].revents & POLLIN)
			{
				if (poll_list[i].fd == listen_sock)
				{
					addrlen = sizeof remoteaddr;
					newfd = accept(listen_sock, (struct sockaddr *)&remoteaddr, &addrlen);
					if (newfd == -1)
						throw std::domain_error("(webserv) Unnaceptable connection.");
					else
					{
						ufds = pollfd();
						ufds.fd = newfd;
						ufds.events = POLLIN;
						poll_list.push_back(ufds);
						verbose(1) << "(webserv) New connection on socket: " << newfd << std::endl;
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

std::string WebServ::gnl(int fd)
{
	int READ_SIZE = 10;
	char read_buffer[READ_SIZE + 1];
	int nbytes, nlbytes;

	read_buffer[READ_SIZE] = '\0';
	nbytes = recv(fd, read_buffer, READ_SIZE, 0);
	if (nbytes == -1)
		throw std::domain_error("(webserv) Something went wrong receiving data.");
	if (nbytes == 0)
		return "";
	for (nlbytes = 0; nlbytes < nbytes; nlbytes++)
		if (read_buffer[nlbytes] == '\n' || read_buffer[nlbytes] == '\r')
			break;
	if (nlbytes == READ_SIZE)
		return std::string(read_buffer) + gnl(fd);
	return std::string(read_buffer);
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
