/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 14:24:28 by fde-capu          #+#    #+#             */
/*   Updated: 2022/04/22 22:43:24 by fde-capu         ###   ########.fr       */
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

void WebServ::init()
{
	struct sockaddr_in si;

	while (server.loop())
	{
		instance.push_back(dftosi(server.val));
	}
	for (size_t i = 0; i < instance.size(); i++)
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

			listen(instance[i].server_socket, 1);
		}
	}
}

WebServ::WebServ(DataFold& u_config)
: config(u_config), server(config.filter("server"))
{
	std::cout << config << std::endl;
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
