/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 14:24:08 by fde-capu          #+#    #+#             */
/*   Updated: 2022/07/07 15:35:07 by fde-capu         ###   ########.fr       */
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
	int			content_length;
	std::string	content_type;

	bool		is_valid;
	int			status;
	std::string status_msg;
	std::string connection;
	std::string location;
	void		header500(); // Server error.
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
	int fd;

	const DataFold operator[] (std::string) const;
	std::string val(std::string) const;

	std::string multipart_type;
	std::string boundary;
	size_t max_size;
	size_t payload_start, payload_end;
	size_t body_start, body_end;

	bool read_more();
	void set_sizes();
	DataFold get_location_config();
	bool is_multitype() const;
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

	int is_301(ws_server_instance&); // Redirect.
	int is_403(ws_server_instance&); // Forbidden.
	int is_405(ws_server_instance&); // Method Not Allowed.
	int is_200(ws_server_instance&); // OK.
	int is_202(ws_server_instance&); // Accepted.
	int is_404(ws_server_instance&); // Not Found.
	int is_413(ws_server_instance&); // Payload Too Large.

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

		void consider_stdin_because_grace();
		void give_up_if_empty_configuration();

		ws_server_instance dftosi(DataFold);
		struct pollfd stdin_to_pollfd();
		void exit_gracefully();
		std::string get_raw_data(int);
		static bool validate_header_entry(std::vector<std::string>&, size_t, bool&);
		static bool validate_header_1st_line(std::string&, size_t, bool&);
		static bool read_1st_line(std::string&, ws_header&, bool&);
		static bool read_host(std::string&, ws_header&, bool&);
		void flush_stdin();
		bool is_port_taken(int) const;
		bool same_port_another_name(const ws_server_instance*) const;
		int catch_connection();
		bool there_is_an_instance(int) const;
		void add_to_poll(int);
		void remove_from_poll(int);
		ws_server_instance choose_instance(ws_header&, int);
		void respond_connection_from(int);
		void load_defaults();
		static void set_non_blocking(int);
		static bool ignore_empty(std::string&);

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

		// Usefull services for everyone!
		static int bind_socket_to_local(int);
		struct pollfd make_pollin_fd(int) const;
		static struct ws_header get_header(const std::string&);
		static std::string get_body(const std::string&);
};

std::ostream & operator<< (std::ostream & o, WebServ const & i);

#endif
