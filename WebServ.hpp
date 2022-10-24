/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 14:24:08 by fde-capu          #+#    #+#             */
/*   Updated: 2022/10/24 16:20:03 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "header.hpp"

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
	std::string transfer_encoding;
	std::string charset;
	std::string expect;
	std::string cookie;

	bool		is_valid;
	int			status;
	std::string status_msg;
	std::string connection;
	std::string location;
	void		header500();

	bool is_post();
	bool is_get();
};
std::ostream & operator<< (std::ostream & o, ws_header const &);

struct ws_server_instance
{
	ws_server_instance();
	ws_header in_header;
	std::string in_body;
	std::vector<int> port;
	std::vector<int> listen_sock;
	DataFold config;
	DataFold root_config;
	int fd;
	Chronometer chronometer;

	const DataFold operator[] (std::string) const;
	std::string val(std::string) const;

	std::string multipart_type;
	std::string multipart_content;
	std::string multipart_content_disposition;
	std::string multipart_name;
	std::string multipart_filename;
	std::string multipart_content_type;
	std::string boundary;

	std::string chunked_content;

	size_t max_size;
	size_t body_start, body_end;
	bool exceeded_limit;
	bool reached_limit;
	int status;

	void set_sizes();
	void set_props();

	int process_post();
	void mount_chunked();
	void mount_multipart();

	DataFold get_location_config() const;
	bool is_multipart() const;
	bool is_chunked() const;
	bool chunk_finished;
	bool multipart_finished;
	bool is_cgi() const;
	bool cgi_flag;
	std::string location_path(const std::string = "") const;
	DataFold server_location_config(const std::string&, std::string = "") const;
	std::string location_get_single(const std::string&, std::string = "") const;
	std::string custom_error(const size_t) const;
};
std::ostream & operator<< (std::ostream & o, ws_server_instance const &);

struct ws_reply_instance
{
	ws_reply_instance();
	void encapsulate();
	void set_code(int, const std::string&);
	void set_redirect(const std::string&);
	char* buffer;
	virtual ~ws_reply_instance();

	ws_header out_header;
	std::string out_body;
	size_t package_length;
	std::string file_name;
	Chronometer chronometer;
	bool first_time;
	std::vector<struct pollfd> poll_list;
	std::string full_path;
	int file_fd;
	int pipe_pc[2];
	int pipe_cp[2];
	pid_t child_pid;
	bool dumping_to_cgi;
	bool getting_from_cgi;
	bool to_work_load;

	int PUT_mock(ws_server_instance&); // So it proceeds for 42 ubuntu_tester.
	int is_501(ws_server_instance&); // Refuses if something is not implemented.
	int is_cgi_exec(ws_server_instance&); // Run!
	int is_301(ws_server_instance&); // Redirect.
	int is_403(ws_server_instance&); // Forbidden.
	int is_405(ws_server_instance&); // Method Not Allowed.
	int is_200(ws_server_instance&); // OK.
	int is_400(ws_server_instance&); // Bad Request.
	int is_201(ws_server_instance&); // Created.
	int is_404(ws_server_instance&); // Not Found.
	int is_413_507_422(ws_server_instance&); //413 Payload Too Large, 507 Insufficient Resources, 422 Unprocessable Entity
	int is_424(ws_server_instance&); // Failed Dependency.

	bool is_working_load(ws_server_instance&);
	bool is_working_save(ws_server_instance&);
	bool is_working_cgi(ws_server_instance&);
	int cgi_prepare(ws_server_instance&, std::string);
	int cgi_pipe(ws_server_instance&, const std::vector<std::string>&);
	void cgi_setenv(ws_server_instance&, std::string);
	bool cgi_dumping(ws_server_instance&);
	bool cgi_receiving();
	int bad_gateway(std::string = "");
	int list_autoindex(std::string, ws_server_instance&);
	void header_from_body();
	bool save_canceled() const;

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
		std::map<int, ws_server_instance> webserver;
		std::vector<struct pollfd> poll_list;
		std::map<int, int> fd_to_port;
		std::vector<int> taken_ports;
		bool lit;
		void hook_it();
		ws_server_instance dftosi(DataFold);
		struct pollfd stdin_to_pollfd();
		void exit_gracefully();
		static bool validate_header_entry(std::vector<std::string>&, size_t, bool&);
		static bool validate_header_1st_line(std::string&, size_t, bool&);
		static bool read_1st_line(std::string&, ws_header&, bool&);
		static bool host_from_header(std::string&, ws_header&, bool&);
		void flush_stdin();
		bool is_port_taken(int) const;
		bool same_port_another_name(const ws_server_instance*) const;
		struct pollfd catch_connection();
		bool is_a_webserv(int) const;
		bool already_initiated(int) const;
		void dup_into_poll(int);
		void remove_from_poll(int);
		ws_server_instance choose_instance(ws_header&, int);
		void load_defaults();
		static bool ignore_empty(std::string&);
		WebServ& operator= (WebServ const & rhs);
		WebServ(WebServ const & src);
		WebServ();
		char* buffer;
		std::map<int, bool> in_ended;
		std::map<int, bool> out_ended;
		std::map<int, bool> body_ok;
		std::map<int, bool> chosen_instance;
		std::map<int, bool> encapsulated;
		std::map<int, std::string> raw;
		std::map<int, ws_header> in_header;
		std::map<int, ws_reply_instance> respond;
		std::map<int, bool> remove_client;
		std::map<int, bool> response_working;
		std::map<int, bool> chosen_response;

	public:
		WebServ(DataFold&);
		~WebServ();
		DataFold getConfig() const;
		DataFold getServer() const;
		std::vector<ws_server_instance> getInstance() const;
		void init();
		void light_up();
		void dispatch(std::map<int, std::pair<bool, bool> >&);

		// Usefull services for everyone!
		static void set_non_blocking(int);
		static int bind_socket_to_local(int);
		static struct pollfd make_in_out_fd(int);
		static struct ws_header get_header(const std::string&);
		static std::string get_body(const std::string&);
};

std::ostream & operator<< (std::ostream & o, WebServ const & i);

#endif
