/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 14:24:28 by fde-capu          #+#    #+#             */
/*   Updated: 2022/05/18 13:31:30 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
#include <iostream>
#include <cstdio>

ws_serv_instance WebServ::dftosi(DataFold df)
{
	ws_serv_instance si;
	si.current_http_header = "HTTP/1.1 200 OK\nConnection: close\nContent-Length: 13\n\n";
	si.current_http_body = "Hello, world!";
	DataFold df_listen(df.get("listen"));
	while (df_listen.loop())
		si.port.push_back(std::atoi(df_listen.val.c_str())); // htons?
	return si;
}

int WebServ::bind_socket_to_local(int u_port)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd, s;
	int yes = 1;

	hints = addrinfo();
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	s = getaddrinfo(NULL, itoa(u_port).c_str(), &hints, &result);
	if (s != 0)
		throw std::domain_error("(webserv) getaddrinfo failed: " + std::string(gai_strerror(s)));

	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
			throw std::domain_error("(webserv) Socket creation failed.");
		if (fcntl(sfd, F_SETFD, O_NONBLOCK) == -1)
			throw std::domain_error("(webserv) Failed to set non-blocking flag.");
		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
			throw std::domain_error("(webserv) Can not reuse socket.");
		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
			break;
		close(sfd);
	}
	if (rp == NULL)
		throw std::domain_error("(webserv) Socket locked.");

	freeaddrinfo(result);
	verbose(1) << "(webserv) Bound fd " << sfd << " to port " << u_port << "." << std::endl;
	return sfd;
}

void WebServ::hook_it()
{
	struct pollfd ufds;
	for (size_t j = 0; j < instance[0].port.size(); j++)
	{

		instance[0].listen_sock.push_back(bind_socket_to_local(instance[0].port[j]));
		ufds = pollfd();
		ufds.fd = instance[0].listen_sock[j];
		ufds.events = POLLIN;
		poll_list.push_back(ufds);
		if (listen(instance[0].listen_sock[j], SOMAXCONN) != 0)
			throw std::domain_error("(webserv) Listening problem.");
	}
	verbose(1) << "(webserv) I'm hooked." << std::endl;
}

struct pollfd WebServ::stdin_to_pollfd()
{ return make_pollin_fd(0); }

struct pollfd WebServ::make_pollin_fd(int newfd)
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
	lit = false;
	return ;
}

// Set stdin.
// {%}
//  Get POLLIN, set pollin_fd.
//  If pollin_fd is any listen_sock of any instance,
//   generate newfd,
//   put newfd to poll,
//   close pollin_fd. // ?
//  If not, it sends data and  awaits response, then
//   receive,
//   analise and route (will generate response),
//   send response,
//   close pollin_fd.

int WebServ::catch_connection()
{
	int TIME_OUT = 0; // 0 = non-blocking, -1 = blocking, N = cycle blocking ms
	int poll_count;
	DataFold event;

	while (lit)
	{
		poll_count = poll(&poll_list[0], poll_list.size(), TIME_OUT);
		if (poll_count == -1)
			throw std::domain_error("(webserv) poll error.");
		for (size_t i = 0; i < poll_list.size(); i++)
			if (poll_list[i].revents & POLLIN)
				return poll_list[i].fd;
	}
	return 0;
}

bool WebServ::there_is_an_instance(int fd) const
{
	for (size_t i = 0; i < instance.size(); i++)
		for (size_t j = 0; j < instance[i].listen_sock.size(); j++)
			if (instance[i].listen_sock[j] == fd)
				return true;
	return false;
}

void WebServ::add_to_poll(int oldfd)
{
	struct sockaddr_storage remoteaddr;
	unsigned int addrlen = sizeof remoteaddr;
	int newfd = accept(oldfd, (struct sockaddr *)&remoteaddr, &addrlen);
	if (newfd == -1)
		throw std::domain_error("(webserv) Unacceptable connection.");
	else
	{
		poll_list.push_back(make_pollin_fd(newfd));
		verbose(1) << "(webserv) New connection on fd (" << oldfd << ")->" << newfd << std::endl;
	}
}

void WebServ::remove_from_poll(int fd)
{
	for (size_t i = 0; i < poll_list.size(); i++)
	{
		if (poll_list[i].fd == fd)
		{
			std::vector<struct pollfd>::iterator position(&poll_list[i]);
			poll_list.erase(position);
			return ;
		}
	}
	throw std::domain_error("(webserv) Cannot remove unlisted fd.");
}

bool WebServ::validate_header_entry(std::vector<std::string>& test, size_t expected_size, bool& is_valid) const
{
	is_valid = is_valid && test.size() == expected_size;
	return is_valid;
}

ws_header WebServ::get_header(const std::string& full_file)
{
	std::string raw_data(full_file);
	ws_header header;
	std::vector<std::string> line;
	std::vector<std::string> carrier;
	bool is_valid;
	is_valid = false;

	remove_all(raw_data, "\r");
	std::string h_block = raw_data.substr(0, raw_data.find("\n\n"));
	verbose(2) << "get_header ==>" << h_block << "<==" << std::endl;
	line = split_trim(h_block, "\n");
	for (size_t i = 0; i < line.size(); i++)
	{
		std::cout << "LINE>" << line[i] << "<" << std::endl;

		if (line[i].length() == 0)
			continue ;

		if (i == 0)
		{
			carrier = split_trim(line[i], "/");
			is_valid = true;
			if (!validate_header_entry(carrier, 3, is_valid))
				break ;
			header.method = carrier[0];
			header.protocol = carrier[1];
			header.protocol_version = carrier[2];
			continue ;
		}
		carrier = split_trim(line[i], ":");
		if (is_equal_insensitive(carrier[0], "host"))
		{
			if (!validate_header_entry(carrier, 3, is_valid))
				break ;
			header.host = carrier[1];
			header.port = carrier[2];
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

	verbose(1) << "method >" << header.method << "<" << std::endl;
	verbose(1) << "protocol >" << header.protocol << "<" << std::endl;
	verbose(1) << "protocol_version >" << header.protocol_version << "<" << std::endl;
	verbose(1) << "host >" << header.host << "<" << std::endl;
	verbose(1) << "port >" << header.port << "<" << std::endl;
	verbose(1) << "user_agent >" << header.user_agent << "<" << std::endl;
	verbose(1) << "accept >" << header.accept << "<" << std::endl;
	verbose(1) << "is_valid >" << header.is_valid << "<" << std::endl;

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

ws_reply_instance::s_ws_reply_instance(ws_header& wsh, std::string& wsb, int fd)
{
	(void)wsh;
	(void)wsb;
	std::cout << "ws_reply_instance fd " << fd << std::endl;
}

void WebServ::respond_connection_from(int fd)
{
	verbose(1) << "(webserv) Got connection from fd " << fd << "." << std::endl;
	std::string raw_data = get_raw_data(fd);
	ws_header in_header = get_header(raw_data);
	if (!in_header.is_valid)
		return remove_from_poll(fd);
	std::string in_body = get_body(raw_data);
	verbose(1) << "BODY >" << in_body << "<" << std::endl;
	ws_reply_instance reply(in_header, in_body, fd);
	if (send(fd, HELLO_WORLD, std::string(HELLO_WORLD).length(), 0) == -1)
		throw std::domain_error("(webserv) Sending response went wrong.");
	remove_from_poll(fd);
}

void WebServ::light_up()
{
	int event;

	lit = true;
	while (lit)
	{
		event = catch_connection();
		if (event == 0)
			return exit_gracefully();
		if (there_is_an_instance(event))
			add_to_poll(event);
		else
			respond_connection_from(event);
	}
}

void WebServ::init()
{
	poll_list.push_back(stdin_to_pollfd());
	hook_it();
	light_up();

	std::string line;
	std::getline(std::cin, line);
}

WebServ::WebServ(DataFold& u_config)
: config(u_config), server(config.filter("server")), lit(false)
{
	while (server.loop())
		instance.push_back(dftosi(server.val));
	return ;
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

std::vector<ws_serv_instance> WebServ::getInstance() const
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
