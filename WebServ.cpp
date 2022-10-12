/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 14:24:28 by fde-capu          #+#    #+#             */
/*   Updated: 2022/10/13 00:39:15 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

// {%}
//  Poll gets POLLIN, set pollin_fd.
//  If pollin_fd is any listen_sock of any instance,
//   generate newfd,
//   put newfd to poll.
//  Poll gets POLLIN again, this time from newfd.
//   receives data, validate data, prepares reply.
//  Poll gets POLLOUT from newfd,
//   send response,
//   removes from poll and close newfd.

WebServ::WebServ(DataFold& u_config)
: config(u_config), server(config.filter("server")), lit(false)
{
	load_defaults();
	while (server.loop())
		instance.push_back(dftosi(server.val));
	if (instance.empty())
		throw std::invalid_argument("(webserv) Configuration is empty.");
	return ;
}

void WebServ::init()
{
	try
	{
		hook_it();
		light_up();
	}
	catch(std::exception& e)
	{
		verbose(CRITICAL) << "(webserv) Error: " << e.what() << std::endl;
		throw e;
	}
}

void WebServ::hook_it()
{
	struct pollfd ufds;

	for (size_t i = 0; i < instance.size(); i++)
	{
		for (size_t j = 0; j < instance[i].port.size(); j++)
		{
			if (!is_port_taken(instance[i].port[j]))
			{
				instance[i].listen_sock.push_back(bind_socket_to_local(instance[i].port[j]));
				ufds = pollfd();
				ufds.fd = instance[i].listen_sock[j];
				ufds.events = POLLIN | POLLOUT;
				fd_to_port[ufds.fd] = instance[i].port[j];
				poll_list.push_back(ufds);
				if (listen(instance[i].listen_sock[j], SOMAXCONN) != 0)
					throw std::domain_error("(webserv) Listening problem.");
				taken_ports.push_back(instance[i].port[j]);
			}
			else if (same_port_another_name(&instance[i]))
				verbose(1) << "(webserv) Warning: multiple servers on same port. Using first." << std::endl;
		}
	}
	poll_list.push_back(stdin_to_pollfd()); // stdin exits gracefully.
	verbose(1) << "(webserv) I'm hooked." << std::endl << std::endl;
}

struct pollfd WebServ::catch_connection()
{
	int TIME_OUT = 0; // 0: non-blocking, -1: blocking, N: cycle blocking ms
	int poll_count;

	poll_count = poll(&poll_list[0], poll_list.size(), TIME_OUT);
	if (poll_count == -1)
		throw std::domain_error("(webserv) Poll error.");
	for (size_t i = 0; i < poll_list.size(); i++)
	{
		if (poll_list[i].revents & POLLIN \
		||  poll_list[i].revents & POLLOUT )
			return poll_list[i];
	}
	struct pollfd out;
	out.fd = -1; // meaning nothing.
	return out;
}

void WebServ::light_up()
{
	int V(1);
	struct pollfd event;

	verbose(V) << "Light up server: " << \
		config.getValStr("server_name") << std::endl;
	verbose(0) << config.getValStr("welcome_message") << std::endl;

	lit = true;
	while (lit)
	{
		event = catch_connection();
		std::cout << *this; // Animation.
		if (event.fd == -1)
			continue ;
		if (event.fd == 0)
			return exit_gracefully();
		if (event.revents & POLLIN)
		{
			if (there_is_an_instance(event.fd))
			{
				verbose(V) << " . . . . . . . . . . . . . . . " << std::endl;
				dup_into_poll(event.fd);
				continue ;
			}
			if (fd_to_si[event.fd].got_pollin)
				continue ;
			verbose(V) << "(light_up) Got POLLIN from " << event.fd << std::endl;
			listen_to(event.fd);
			fd_to_si[event.fd].got_pollin = true;
		}
		if (event.revents & POLLOUT || fd_to_si[event.fd].got_pollout)
		{
			if (fd_to_si.find(event.fd) == fd_to_si.end())
				fd_to_si[event.fd] = ws_server_instance();
			if (fd_to_si[event.fd].got_pollout)
				continue ;
			fd_to_si[event.fd].got_pollout = true;
			verbose(V) << "(light_up) Got POLLOUT from " << event.fd << std::endl;
			if (!fd_to_si[event.fd].got_pollin)
				continue ;
			send_response(event.fd);
			close(event.fd);
			remove_from_poll(event.fd);
		}
	}
}

void WebServ::listen_to(int fd)
{
	int V(1);
	std::string body;
	ws_header in_header;
	Chronometer time_out;
	CircularBuffer raw(fd);
	std::string encapsulated;

	verbose(V) << "(listen_to) Getting data from fd " << fd \
		<< "." << std::endl;

	while (!in_header.is_valid)
	{
		if (time_out > TIME_OUT_MSEC)
		{
			verbose(V) << "(listen_to) Timeout! << " << time_out << \
				" > Incomplete header" << std::endl;
			return respond_timeout(fd);
		}
		raw.receive_until("\r\n\r\n");
		in_header = get_header(raw.output);
	}

	verbose(2) << "(listen_to) Got header:" << std::endl << \
		in_header << std::endl;

	fd_to_si[fd] = choose_instance(in_header, fd_to_port[fd]);
	fd_to_si[fd].in_body = get_body(raw.receive_until_eof());
	fd_to_si[fd].set_props();
	fd_to_si[fd].set_sizes();
	fd_to_si[fd].fd = fd;
}

void WebServ::send_response(int fd)
{
	ws_reply_instance respond(fd_to_si[fd]); // ...oonn...
	respond.encapsulate();
	if (send(fd, respond.out_body.c_str(),
		respond.package_length, 0) == -1)
		throw std::domain_error("(webserv) Sending response went wrong.");
}

ws_reply_instance::ws_reply_instance(ws_server_instance& si)
{
	*this = ws_reply_instance();
	out_body = "";

	// Order matters.
	if (PUT_mock(si)) return ; // 200 but mocked.
	if (is_501(si)) return ; // Is it not implemented?
	if (is_301(si)) return ; // Redirect.
	if (is_405(si)) return ; // Bad method.
	if (is_403(si)) return ; // Forbidden.
	if (is_404(si)) return ; // Not found. GET. (must be before 2xx)
	if (is_424(si)) return ; // Not met dependency. Used when client expects 100-continue.
	if (is_413_507_422(si)) return ; // 413 Too Large, 507 No resource, 422 Unprocessable.
	if (is_400(si)) return ; // 400 Bad Request. POST w/o filename.
	if (is_cgi_exec(si)) return ; // Runs CGI and returns accordingly. 202 Accepted.
	if (is_201(si)) return ; // Created (POST) and saves data.
	if (is_200(si)) return ; // Ok (GET) and loads file.

	set_code(420, "Enhance Your Calm");
	out_body = TemplateError::page(420, si.custom_error(420));
}

ws_server_instance WebServ::choose_instance(ws_header& in, int in_port)
{
	int V(1);
	ws_server_instance si;
	ws_server_instance *choose;

	choose = 0;
	for (size_t i = 0; i < instance.size(); i++)
	{
		for (size_t j = 0; j < instance[i].port.size(); j++)
		{
			if (instance[i].port[j] == in_port)
			{
				if (!choose)
				{
					choose = &instance[i];
				}
				if (instance[i].config.getValStr("server_name") \
					== in.host)
				{
					choose = &instance[i];
					break ;
				}
			}
		}
	}
	if (!choose)
		throw std::domain_error(\
			"(choose_instance) Cannot define responding instance.");
	si = *choose;
	si.in_header = in;
	si.in_header.port = in_port;
	if (si.config.get("index").empty())
		si.config.set("index", config.getValStr("index"));
	si.root_config.push_back("root", config.getValStr \
		("working_directory"));
	si.root_config.push_back("client_max_body_size", config.getValStr \
		("client_max_body_size"));

	verbose(V) << "(choose_instance) Responding as " << \
		choose->config.getValStr("server_name") << ":" << in_port << \
		"." << std::endl;

	return si;
}

void WebServ::set_non_blocking(int sock)
{
	int opts;
	opts = O_NONBLOCK;
	if (fcntl(sock, F_SETFL, opts) == -1)
		throw std::domain_error("(webserv) Could not set non-blocking flag.");
}

void WebServ::remove_from_poll(int fd)
{
	for (size_t i = 0; i < poll_list.size(); i++)
	{
		if (poll_list[i].fd == fd)
		{
			std::vector<struct pollfd>::iterator position(&poll_list[i]);
			poll_list.erase(position);
			break ;
		}
	}
	for (std::map<int, ws_server_instance>::iterator it = fd_to_si.begin(); it != fd_to_si.end(); it++)
	{
		if (it->first == fd)
		{
			fd_to_si.erase(it);
			break ;
		}
	}
}

void WebServ::dup_into_poll(int oldfd)
{
	int V(1);
	struct sockaddr_storage remoteaddr;
	unsigned int addrlen = sizeof remoteaddr;
	int newfd = accept(oldfd, (struct sockaddr *)&remoteaddr, &addrlen);
	if (newfd == -1)
		throw std::domain_error("(webserv) Unacceptable connection.");
	set_non_blocking(newfd);
	poll_list.push_back(make_in_out_fd(newfd));
	fd_to_port[newfd] = fd_to_port[oldfd];

	verbose(V) << "(webserv) Connection from fd (" << oldfd << \
		")->" << newfd << "." << std::endl;
}
