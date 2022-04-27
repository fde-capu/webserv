/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 14:24:08 by fde-capu          #+#    #+#             */
/*   Updated: 2022/04/27 07:22:31 by fde-capu         ###   ########.fr       */
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

# define HELLO_WORLD "HTTP/1.1 200 OK\nConnection: close\nContent-Length: 14\n\nHello, world!\n";

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
		void hook_it();

		void listen_on(int);
		int bind_socket_to_local(int);
		void accept_connection();

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
