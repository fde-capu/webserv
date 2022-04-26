/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 14:24:08 by fde-capu          #+#    #+#             */
/*   Updated: 2022/04/26 02:38:51 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "header.hpp"
# include "DataFold.hpp"
# include <iostream>
# include <vector>
# include <arpa/inet.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <unistd.h>

typedef struct struct_server_instance
{
	std::string current_http_header;
	std::string current_http_body;
	int server_socket;
	std::vector<struct sockaddr_in> server_address;
	DataFold attribute;
} server_instance;

class WebServ
{
	private:
		DataFold config;
		DataFold server;
		std::vector<server_instance> instance;

		server_instance dftosi(DataFold);
		size_t port_count();
		WebServ();

		void bind_ports();
		void listen_on();
		void hook_it();

	public:
		WebServ(DataFold&);
		WebServ(WebServ const & src);
		WebServ & operator= (WebServ const & rhs);
		~WebServ();
		DataFold getConfig() const;
		DataFold getServer() const;
		std::vector<server_instance> getInstance() const;
		void init();
};

std::ostream & operator<< (std::ostream & o, WebServ const & i);

#endif
