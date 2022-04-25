/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 14:24:28 by fde-capu          #+#    #+#             */
/*   Updated: 2022/04/25 22:10:12 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
#include <iostream>

server_instance WebServ::dftosi(DataFold df)
{
	server_instance si;
	si.current_http_header = "";
	si.current_http_body = "";
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

void WebServ::listen_on()
{
	for (size_t i = 0; i < port_count(); i++)
		listen(instance[i].server_socket, 1);
}

void WebServ::hook_it()
{
	struct sockaddr_in *communication_layer;
	int client_socket;
	struct addrinfo hints;
	struct addrinfo *result;
	struct addrinfo *info;
	int sign;
	char netstr[INET6_ADDRSTRLEN];
	struct sockaddr_in *port_in;
	struct sockaddr_in6 *port_in6;

	while (1)
	{
		for(size_t i = 0; i < instance.size(); i++)
		{
			*communication_layer = calloc(1, sizeof(struct sockaddr_in));
			client_socket = accept(
				instance[i].server_socket,
				(struct sockaddr*)&communication_layer,
				(socklen_t *)&communication_layer->sin_addr.s_addr
			);
			memset(&hints, 0, sizeof(struct addrinfo));
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_DGRAM;
			hints.ai_flags = AI_PASSIVE;
			hints.ai_protocol = 0;
			hints.ai_canonname = NULL;
			hints.ai_addr = NULL;
			hints.ai_next = NULL;
			sign = getaddrinfo(NULL, instance[i].server_socket, &hints, &result);
			if (sign != 0)
			{
				verbose(0) << SERVER_SIGN " " ERROR FAILEDADDRINGO << std::endl;
				continue ;
			}
		}
		for(info = result; info != NULL; info = info->ai_next)
		{
			if (info->ai_addr->sa_family == AF_INET)
			{
				*port_in = (struct sockaddr_in *)info->ai_addr;
				verbose(0) << "Connection from: " << inet_ntop(AF_INET, &port_in->sin_addr, netstr, sizeof(netstr)) << std::endl;
			}
			else if (info->ai_addr->sa_family == AF_INET6)
			{
				*port_in6 = (struct sockaddr_in6 *)info->ai_addr;
				verbose(0) << "Connection from: " << inet_ntop(AF_INET6, &port_in6->sin6_addr, netstr, sizeof(netstr)) << std::endl;
			}
		}

		verbose(0) << SERVER_SIGN << " Family: %d\n" << communication_layer->sin_family;
		verbose(0) << SERVER_SIGN << " Port: %d\n" << ntohs(communication_layer->sin_port);
		verbose(0) << SERVER_SIGN << " in_addr: %d\n" << communication_layer->sin_addr.s_addr;
		verbose(0) << SERVER_SIGN << "--> %s <--\n" << http_header;

		send(client_socket, instance[i].current_http_header, strlen(instance[i].current_http_header), 0);
		close(client_socket);
		free(communication_layer);
		//////////
//		std::cout << "!!!!" << std::endl;
//		break ;
	}
}

void WebServ::init()
{
	bind_ports();
	listen_on();
	hook_it();
}

WebServ::WebServ(DataFold& u_config)
: config(u_config), server(config.filter("server"))
{
	verbose(1) << "Loading server..." << std::endl;
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
