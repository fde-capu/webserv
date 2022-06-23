/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 14:24:08 by fde-capu          #+#    #+#             */
/*   Updated: 2022/06/23 16:14:07 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "header.hpp"

# define HELLO_WORLD "HTTP/1.1 200 OK\nConnection: close\nContent-Length: 14\n\nHello, world!\n"

struct ws_header
{
	ws_header();
	std::string method;
	std::string directory;
	std::string protocol;
	std::string host;
	int			port;
	std::string user_agent;
	std::string accept;
	bool		is_valid;
	int			status;
	std::string status_msg;
	std::string connection;
	std::string location;
	void		header500();
};
std::ostream & operator<< (std::ostream & o, ws_header const &);

struct ws_server_instance
{
	ws_header in_header;
	std::string in_body;
	std::vector<int> port;
	std::vector<int> listen_sock;
	DataFold config;
	DataFold root_config;

	const DataFold operator[] (std::string) const;
	std::string val(std::string) const;
};
std::ostream & operator<< (std::ostream & o, ws_server_instance const &);

struct ws_reply_instance
{
	ws_header out_header;
	std::string out_body;
	size_t package_length;
	ws_reply_instance();
	std::string encapsulate();
	void set_code(int, const std::string&);
	void set_redirect(const std::string&);

	ws_reply_instance(ws_server_instance&); // Arg may be std::string&
	private:								// and auto-convert
		ws_reply_instance(std::string&);	// to ws_server_instance&.
};

class WebServ
{
	private:
		DataFold config;
		DataFold server;
		std::vector<ws_server_instance> instance;
		std::vector<struct pollfd> poll_list;
		std::map<int, int> fd_to_port;
		std::vector<int> taken_ports;
		bool lit;

		WebServ();

		void hook_it();

		ws_server_instance dftosi(DataFold);
		struct pollfd stdin_to_pollfd();
		void exit_gracefully();
		std::string get_raw_data(int);
		static bool validate_header_entry(std::vector<std::string>&, size_t, bool&);
		void flush_stdin();
		bool is_port_taken(int) const;
		bool same_port_another_name(const ws_server_instance*) const;
		int catch_connection();
		bool there_is_an_instance(int) const;
		void add_to_poll(int);
		void remove_from_poll(int);
		ws_server_instance choose_instance(std::string&, int);
		void respond_connection_from(int);
		void load_defaults();

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

	public: // Usefull services that needs sharing with CgiWrapper.
		static int bind_socket_to_local(int);
		struct pollfd make_pollin_fd(int) const;
		static struct ws_header get_header(const std::string&);
		static std::string get_body(const std::string&);
};

std::ostream & operator<< (std::ostream & o, WebServ const & i);

#endif
