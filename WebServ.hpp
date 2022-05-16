/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 14:24:08 by fde-capu          #+#    #+#             */
/*   Updated: 2022/05/16 15:12:52 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "header.hpp"
# include "DataFold.hpp"
# include "CircularBuffer.hpp"
# include <iostream>
# include <vector>
# include <arpa/inet.h>
// # include <sys/types.h>
//# include <sys/socket.h>
# include <netdb.h>
# include <unistd.h>
# include <poll.h>
# include <fcntl.h>

# define MAX_EVENTS 64
# define HELLO_WORLD "HTTP/1.1 200 OK\nConnection: close\nContent-Length: 14\n\nHello, world!\n";

typedef struct struct_ws_serv_instance
{
	std::string current_http_header;
	std::string current_http_body;
	std::vector<int> port;
	std::vector<int> listen_sock;
} ws_serv_instance;

class WebServ
{
	private:
		DataFold config;
		DataFold server;
		std::vector<ws_serv_instance> instance;
		std::vector<struct pollfd> poll_list;

		WebServ();

		void hook_it();

		ws_serv_instance dftosi(DataFold);
		int bind_socket_to_local(int);
		struct pollfd stdin_to_pollfd();
		struct pollfd make_pollin_fd(int);
		void exit_gracefully();

	public:
		WebServ(DataFold&);
		WebServ(WebServ const & src);
		WebServ & operator= (WebServ const & rhs);
		~WebServ();
		DataFold getConfig() const;
		DataFold getServer() const;
		std::vector<ws_serv_instance> getInstance() const;
		void init();
		void light_up();
};

std::ostream & operator<< (std::ostream & o, WebServ const & i);

#endif
