/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ_helpers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/18 15:25:13 by fde-capu          #+#    #+#             */
/*   Updated: 2022/06/22 01:29:15 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

bool WebServ::is_port_taken(int port) const
{
	for (size_t i = 0; i < taken_ports.size(); i++)
		if (taken_ports[i] == port)
			return true;
	return false;
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

bool WebServ::validate_header_entry(std::vector<std::string>& test, size_t expected_size, bool& is_valid)
{
	is_valid = is_valid && test.size() == expected_size;
	return is_valid;
}

struct ws_header WebServ::get_header(const std::string& full_file)
{
	std::string raw_data(full_file);
	ws_header header;
	std::vector<std::string> line;
	std::vector<std::string> carrier;
	bool is_valid;
	is_valid = false;

	remove_all(raw_data, "\r");
	std::string h_block = raw_data.substr(0, raw_data.find("\n\n"));
	verbose(1) << "get_header ==>" << h_block << "<==" << std::endl;
	line = split_trim(h_block, "\n");
	for (size_t i = 0; i < line.size(); i++)
	{
		verbose(1) << "(webserv) LINE>" << line[i] << "<" << std::endl;

		if (line[i].length() == 0)
			continue ;

		if (i == 0)
		{
			carrier = split_trim(line[i], " ");
			is_valid = true;
			if (!validate_header_entry(carrier, 3, is_valid))
				break ;
			header.method = carrier[0];
			header.directory = carrier[1];
			header.protocol = carrier[2];
			continue ;
		}
		carrier = split_trim(line[i], ":");
		if (is_equal_insensitive(carrier[0], "host"))
		{
			if (validate_header_entry(carrier, 3, is_valid))
			{
				header.host = carrier[1];
				header.port = carrier[2];
			}
			else
			{
				is_valid = true;
				if (validate_header_entry(carrier, 2, is_valid))
					header.host = carrier[1];
				else
					break ;
			}
		}

		if (is_equal_insensitive(carrier[0], "user-agent"))
		{
			if (!validate_header_entry(carrier, 2, is_valid))
				break ;
			header.user_agent = carrier[1];
		}

		if (is_equal_insensitive(carrier[0], "accept"))
		{
			if (!validate_header_entry(carrier, 2, is_valid))
				break ;
			header.accept = carrier[1];
		}
	}
	header.is_valid = is_valid;

	verbose(2) << "(webserv) method >" << header.method << "<" << std::endl;
	verbose(2) << "(webserv) directory >" << header.directory << "<" << std::endl;
	verbose(2) << "(webserv) protocol >" << header.protocol << "<" << std::endl;
	verbose(2) << "(webserv) host >" << header.host << "<" << std::endl;
	verbose(2) << "(webserv) port >" << header.port << "<" << std::endl;
	verbose(2) << "(webserv) user_agent >" << header.user_agent << "<" << std::endl;
	verbose(2) << "(webserv) accept >" << header.accept << "<" << std::endl;
	verbose(2) << "(webserv) is_valid >" << header.is_valid << "<" << std::endl;

	return header;
}

std::string WebServ::get_body(const std::string& full_file)
{
	std::string raw_data(full_file);
	remove_all(raw_data, "\r");
	size_t body_p = raw_data.find("\n\n") + 2;
	return body_p == std::string::npos ? raw_data : raw_data.substr(body_p);
}

std::string WebServ::get_raw_data(int fd)
{
	CircularBuffer buffer(fd);
	buffer.receive_until_eof();
	std::string raw_data = buffer.output;
	verbose(2) << "-->" << raw_data << "<--" << std::endl;
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
