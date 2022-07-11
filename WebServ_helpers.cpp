/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ_helpers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/18 15:25:13 by fde-capu          #+#    #+#             */
/*   Updated: 2022/07/11 15:42:49 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

bool ws_server_instance::read_more()
{
	verbose(1) << "(read_more) " << in_header.directory << \
		" accepting at most " << max_size << " bytes." << std::endl;
	verbose(1) << "(read_more) Payload start: " << payload_start \
		<< ", end: " << payload_end << "." << std::endl;
	verbose(1) << "(read_more) Body start: " << body_start \
		<< ", end: " << body_end << "." << std::endl;
	verbose(1) << "(read_more) From fd: " << fd << std::endl;

	if (is_multitype())
		in_body = in_body.substr(body_start, body_end - body_start);

	verbose(1) << "=== SI ===" << std::endl << *this;
	CircularBuffer more(fd);

	verbose(1) << "(read_more) in_body: >>" << in_body << "<<" \
		<< std::endl;
	return false;
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
	bool out = false;
	for (size_t i = 0; i < instance.size(); i++)
	{
		for (size_t j = 0; j < instance[i].port.size(); j++)
		{
			for (size_t k = 0; k < candidate->port.size(); k++)
			{
				if ((instance[i].port[j] == candidate->port[k]) && \
						(instance[i].config.getValStr("server_name") \
						 != candidate->config.getValStr("server_name")))
					out = true;
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

struct pollfd WebServ::stdin_to_pollfd()
{ return make_pollin_fd(0); }

struct pollfd WebServ::make_pollin_fd(int newfd) const
{
	struct pollfd ufds;

	ufds = pollfd();
	ufds.fd = newfd;
	ufds.events = POLLIN;
	return ufds;
}

void WebServ::exit_gracefully()
{
	flush_stdin();
	verbose(1) << "(webserv) Exit gracefully. Thanks!" << std::endl;
	verbose(1) << config.getValStr("bye_message") << std::endl;
	lit = false;
	return ;
}

bool WebServ::there_is_an_instance(int fd) const
{
	for (size_t i = 0; i < instance.size(); i++)
		for (size_t j = 0; j < instance[i].listen_sock.size(); j++)
			if (instance[i].listen_sock[j] == fd)
				return true;
	return false;
}

bool WebServ::validate_header_1st_line
	(std::string& test_str, size_t expected_size, bool& is_valid)
{
	std::vector<std::string> carrier;

	carrier = split_trim(test_str, " ");
	is_valid = true;
	return validate_header_entry(carrier, expected_size, is_valid);
}

bool WebServ::validate_header_entry
	(std::vector<std::string>& test, size_t expected_size, bool& is_valid)
{
	is_valid = is_valid && test.size() == expected_size;
	return is_valid;
}

ws_header::ws_header()
{
	port = 0;
	is_valid = false;
	status = 0;
	content_length = 0;
}

bool WebServ::ignore_empty(std::string& line)
{ return line.length() == 0; }

bool WebServ::read_1st_line(std::string& line, ws_header& header, bool& is_valid)
{
	std::vector<std::string> carrier;

	if (validate_header_1st_line(line, 3, is_valid))
	{
		carrier = split_trim(line, " ");
		header.method = carrier[0];
		header.directory = carrier[1];
		header.protocol = carrier[2];
	}
	else
		is_valid = false;
	return is_valid;
}

bool WebServ::read_host(std::string& line, ws_header& header, bool& is_valid)
{
	std::vector<std::string> carrier;
	bool prev_valid = is_valid;

	carrier = split_trim(line, ":");
	if (is_equal_insensitive(carrier[0], "host"))
	{
		if (validate_header_entry(carrier, 3, is_valid))
		{
			header.host = carrier[1];
			header.port = atoi(carrier[2].c_str());
		}
		else
		{
			is_valid = prev_valid;
			if (validate_header_entry(carrier, 2, is_valid))
				header.host = carrier[1];
		}
		return is_valid;
	}
	return false;
}

struct ws_header WebServ::get_header(const std::string& full_file)
{
	ws_header header;
	std::vector<std::string> line;
	std::vector<std::string> carrier;
	bool is_valid;
	is_valid = false;

	std::string h_block = full_file.substr(0, full_file.find("\r\n\r\n"));
	line = split_trim(h_block, "\r\n");
	for (size_t i = 0; i < line.size(); i++)
	{
		verbose(1) << "(webserv) LINE >>" << line[i] << "<<" << std::endl;
		if (ignore_empty(line[i])) continue ;
		if (i == 0 && read_1st_line(line[i], header, is_valid)) continue ;
		if (read_host(line[i], header, is_valid)) continue ;
		carrier = split_trim(line[i], ":");
		if (!validate_header_entry(carrier, 2, is_valid)) continue ;
		if (is_equal_insensitive(carrier[0], "user-agent")) header.user_agent = carrier[1];
		if (is_equal_insensitive(carrier[0], "accept")) header.accept = carrier[1];
		if (is_equal_insensitive(carrier[0], "content-length")) header.content_length = atoi(carrier[1].c_str());
		if (is_equal_insensitive(carrier[0], "content-type")) header.content_type = carrier[1];
	}
	header.is_valid = is_valid;
	verbose(3) << header;
	return header;
}

std::string WebServ::get_body(const std::string& full_file)
{
	size_t pos = full_file.find("\r\n\r\n");
	return pos == std::string::npos ? "" : full_file.substr(pos + 4);
}

std::string WebServ::get_raw_data(int fd)
{
	CircularBuffer buffer(fd);
	buffer.receive_until_eof();
	std::string raw_data(buffer.output);
	verbose(1) << "(WebServ) RAW_DATA >>" << raw_data << "<<" << std::endl;
	return raw_data;
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

std::vector<ws_server_instance> WebServ::getInstance() const
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

void WebServ::flush_stdin()
{
	std::string line;
	std::getline(std::cin, line);
}

std::ostream & operator<< (std::ostream & o, ws_header const & wsh)
{
	o << "ws_header | method     | " << wsh.method << std::endl;
	o << "ws_header | directory  | " << wsh.directory << std::endl;
	o << "ws_header | protocol   | " << wsh.protocol << std::endl;
	o << "ws_header | host       | " << wsh.host << std::endl;
	o << "ws_header | port       | " << wsh.port << std::endl;
	o << "ws_header | user_agent | " << wsh.user_agent << std::endl;
	o << "ws_header | accept     | " << wsh.accept << std::endl;
	o << "ws_header | is_valid   | " << wsh.is_valid << std::endl;
	o << "ws_header | status     | " << wsh.status << std::endl;
	o << "ws_header | status_msg | " << wsh.status_msg << std::endl;
	o << "ws_header | connection | " << wsh.connection << std::endl;
	o << "ws_header | con-length | " << wsh.content_length << std::endl;
	o << "ws_header | con-type   | " << wsh.content_type << std::endl;
	return o;
}

std::ostream & operator<< (std::ostream & o, ws_server_instance const & wssi)
{
	o << "ws_server_instance | port        | ";
	for (size_t i = 0; i < wssi.port.size(); i++)
	{ o << wssi.port[i] << " "; }
	o << std::endl;
	o << "ws_server_instance | listen_sock | ";
	for (size_t i = 0; i < wssi.listen_sock.size(); i++)
	{ o << wssi.listen_sock[i] << " "; }
	o << std::endl;
	o << "ws_server_instance | config      | " << wssi.config << std::endl;
	o << "ws_server_instance | in_header   :" << std::endl << wssi.in_header \
		<< std::endl;
	o << "ws_server_instance | in_body     :" << std::endl << ">>" << \
		wssi.in_body << "<<" << std::endl;
	o << "ws_server_instance | multipart_type | " << wssi.multipart_type << std::endl;
	o << "ws_server_instance | boundary >>" << wssi.boundary << "<<" << std::endl;
	o << "ws_server_instance | max_size | " << wssi.max_size << std::endl;
	return o;
}

const DataFold ws_server_instance::operator[] (std::string df_query) const
{ return this->config.get_val(df_query); }

std::string ws_server_instance::val(std::string key) const
{ return this->config.get<DataFold>(key)[key]; }

void ws_reply_instance::set_code(int code_n, const std::string& u_output)
{
	out_header.status = code_n;
	out_header.status_msg = u_output;
}

void ws_reply_instance::set_redirect(const std::string& target)
{ out_header.location = target; }

void WebServ::load_defaults()
{
	if (config.getValStr("server_name") == "")
		config.set("server_name", DEFAULT_SERVER_NAME);
	if (config.getValStr("welcome_message") == "")
		config.set("welcome_message", DEFAULT_WELCOME_MESSAGE);
	if (config.getValStr("bye_message") == "")
		config.set("bye_message", DEFAULT_BYE);
	if (config.getValStr("index") == "")
		config.set("index", DEFAULT_INDEX);
}

std::string ws_reply_instance::encapsulate()
{
	std::string out = "";

	out += out_header.protocol + " ";
	out += itoa(out_header.status) + " " + out_header.status_msg + "\n";
	if (out_header.connection != "")
		out += "Connection: " + out_header.connection + "\n";
	out += "Content-Length: " + itoa(out_body.length()) + "\n";
	if (out_header.location != "")
		out += "Location: " + out_header.location + "\n";
	out += "\n";
	out += out_body;
	package_length = out.length();
	return out;
}

void ws_header::header500()
{
	method = "";
	directory = "";
	protocol = "HTTP/1.1";
	status = 500;
	status_msg = "Internal Server Error";
	connection = "close";
}

ws_reply_instance::ws_reply_instance()
{
	out_header.header500();
}
