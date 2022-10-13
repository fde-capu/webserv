/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ_ports.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/04 15:40:40 by fde-capu          #+#    #+#             */
/*   Updated: 2022/10/13 16:43:17 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

int WebServ::bind_socket_to_local(int u_port)
{
	int V(2);
	struct addrinfo hints;
	struct addrinfo *result, *rp; // rp = read pointer
	int sfd(0), s;
	int yes = 1;

	hints = addrinfo();
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	s = getaddrinfo(NULL, itoa(u_port).c_str(), &hints, &result);
	if (s != 0)
	{
		freeaddrinfo(result);
		throw std::domain_error("(bind_socket_to_local) getaddrinfo failed: " + std::string(gai_strerror(s)));
	}

	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
		{
			freeaddrinfo(result);
			throw std::domain_error("(bind_socket_to_local) Socket creation failed.");
		}
		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		{
			freeaddrinfo(result);
			throw std::domain_error("(bind_socket_to_local) Could not unlock the socket.");
		}
		try
		{
			set_non_blocking(sfd);
		}
		catch (std::exception& e)
		{
			freeaddrinfo(result);
			throw e;
		}
		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
			break;
		close(sfd);
	}
	freeaddrinfo(result);
	if (rp == NULL)
		throw std::domain_error("(bind_socket_to_local) Socket locked.");
	verbose(V) << "(bind_socket_to_local) Bound fd " << sfd << " to port " << u_port << "." << std::endl;
	return sfd;
}

bool WebServ::is_port_taken(int port) const
{
	for (size_t i = 0; i < taken_ports.size(); i++)
		if (taken_ports[i] == port)
			return true;
	return false;
}

bool WebServ::same_port_another_name(const ws_server_instance* candidate) const
{
	bool out = true;
	for (size_t i = 0; i < instance.size(); i++)
	{
		for (size_t j = 0; j < instance[i].port.size(); j++)
		{
			for (size_t k = 0; k < candidate->port.size(); k++)
			{
				if ((instance[i].port[j] == candidate->port[k]) && \
						(instance[i].config.getValStr("server_name") \
						 != candidate->config.getValStr("server_name")))
					out = false;
			}
		}
	}
	return out;
}

ws_server_instance WebServ::dftosi(DataFold df)
{
	ws_server_instance si;
	int port;
	DataFold df_listen(df.get("listen"));
	while (df_listen.loop())
	{
		port = std::atoi(df_listen.val.c_str());
		si.port.push_back(port); // htons?
		si.config = df;
	}
	return si;
}
