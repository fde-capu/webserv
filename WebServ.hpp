/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 14:24:08 by fde-capu          #+#    #+#             */
/*   Updated: 2022/05/18 15:04:06 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "header.hpp"
# include "DataFold.hpp"
# include "CircularBuffer.hpp"
# include <algorithm>
# include <iostream>
# include <vector>
# include <arpa/inet.h>
// # include <sys/types.h>
//# include <sys/socket.h>
# include <netdb.h>
# include <unistd.h>
# include <poll.h>
# include <fcntl.h>

# define HELLO_WORLD "HTTP/1.1 200 OK\nConnection: close\nContent-Length: 14\n\nHello, world!\n"

struct ws_header
{
	std::string method;
	std::string protocol;
	std::string protocol_version;
	std::string host;
	std::string port;
	std::string user_agent;
	std::string accept;
	bool		is_valid;
};

struct ws_server_instance
{
	std::string current_http_header;
	std::string current_http_body;
	std::vector<int> port;
	std::vector<int> listen_sock;
	DataFold config;
};

struct ws_reply_instance
{
	ws_header out_header;
	std::string out_body;
	ws_server_instance* config;
	int fd;
	std::string encapsulate();
	size_t cap_length;
	ws_reply_instance(ws_header&, std::string&, int, const ws_server_instance*);
};

class WebServ
{
	private:
		DataFold config;
		DataFold server;
		std::vector<ws_server_instance> instance;
		std::vector<struct pollfd> poll_list;
		std::map<int, const ws_server_instance*> fd_to_instance;
		bool lit;

		WebServ();

		void hook_it();

		ws_server_instance dftosi(DataFold);
		int bind_socket_to_local(int);
		struct pollfd stdin_to_pollfd();
		struct pollfd make_pollin_fd(int);
		void exit_gracefully();
		ws_header get_header(const std::string&);
		std::string get_body(const std::string&);
		std::string get_raw_data(int);
		bool validate_header_entry(std::vector<std::string>&, size_t, bool&) const;
		const ws_server_instance* same_instance_as(int) const;

	public:
		WebServ(DataFold&);
		WebServ(WebServ const & src);
		WebServ & operator= (WebServ const & rhs);
		~WebServ();
		DataFold getConfig() const;
		DataFold getServer() const;
		std::vector<ws_server_instance> getInstance() const;
		void init();
		void light_up();

		int catch_connection();
		bool there_is_an_instance(int) const;
		void add_to_poll(int);
		void remove_from_poll(int);
		void respond_connection_from(int);
};

std::ostream & operator<< (std::ostream & o, WebServ const & i);

#endif
