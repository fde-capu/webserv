/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 14:24:28 by fde-capu          #+#    #+#             */
/*   Updated: 2022/06/22 12:17:46 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

// Set stdin.
// {%}
//  Get POLLIN, set pollin_fd.
//  If pollin_fd is any listen_sock of any instance,
//   generate newfd,
//   put newfd to poll.
//  If not, it sends data and awaits response, then
//   receive,
//   analise and route (will generate response),
//   send response,
//   close pollin_fd.

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
			throw std::domain_error("(webserv) Could not unlock the socket.");
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

	for (size_t i = 0; i < instance.size(); i++)
	{
		for (size_t j = 0; j < instance[i].port.size(); j++)
		{
			if (!is_port_taken(instance[i].port[j]))
			{
				instance[i].listen_sock.push_back(bind_socket_to_local(instance[i].port[j]));
				ufds = pollfd();
				ufds.fd = instance[i].listen_sock[j];
				ufds.events = POLLIN;
				poll_list.push_back(ufds);
				fd_to_instance[ufds.fd] = &instance[i];
				if (listen(instance[i].listen_sock[j], SOMAXCONN) != 0)
					throw std::domain_error("(webserv) Listening problem.");
				taken_ports.push_back(instance[i].port[j]);
			}
			else if (!same_port_another_name(&instance[i]))
				verbose(1) << "(webserv) Warning: multiple servers configured on same port. Only the first will be used." << std::endl;
		}
	}
	verbose(1) << "(webserv) I'm hooked." << std::endl << std::endl;
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

void WebServ::add_to_poll(int oldfd)
{
	struct sockaddr_storage remoteaddr;
	unsigned int addrlen = sizeof remoteaddr;
	int newfd = accept(oldfd, (struct sockaddr *)&remoteaddr, &addrlen);
	if (newfd == -1)
		throw std::domain_error("(webserv) Unacceptable connection.");
	poll_list.push_back(make_pollin_fd(newfd));
	fd_to_instance[newfd] = fd_to_instance[oldfd];
	verbose(1) << "(webserv) New connection on fd (" << oldfd << ")->" << newfd << "." << std::endl;
}

int WebServ::catch_connection()
{
	int TIME_OUT = 0; // 0 = non-blocking, -1 = blocking, N = cycle blocking ms
	int poll_count;
	DataFold event;

	while (lit)
	{
		poll_count = poll(&poll_list[0], poll_list.size(), TIME_OUT);
		if (poll_count == -1)
			throw std::domain_error("(webserv) Poll error.");
		for (size_t i = 0; i < poll_list.size(); i++)
			if (poll_list[i].revents & POLLIN)
				return poll_list[i].fd;
	}
	return 0;
}

ws_reply_instance::ws_reply_instance()
{
	out_header.method = "";
	out_header.directory = "/";
	out_header.protocol = "HTTP/1.1";
	out_header.status = 500;
	out_header.status_msg = "Internal Server Error";
	out_header.connection = "close";
}

ws_reply_instance::ws_reply_instance(ws_server_instance& si)
{
	DataFold loops;
	std::string root;
	std::string file_name;

	*this = ws_reply_instance();
	verbose(1) << "[THINK] " << std::endl;
	verbose(1) << si << std::endl;
	loops = si["index"];
	out_body = "";
	while (loops.loop())
	{
		root = si.root_config.getValStr("root") + "/" + si.val("root");
		file_name = root + si.in_header.directory + "/" + loops.val;
		stool.remove_dup_char(file_name, '/');
		FileString from_file(file_name.c_str());
		out_body = from_file.content();
		if (out_body != "")
		{
			set_code(200, "OK");
			return;
		}
	}
	std::cout << "FILE NOT FOUND" << std::endl;
}

std::string ws_reply_instance::encapsulate()
{
	std::string out = "";

	out += out_header.protocol + " ";
	out += itoa(out_header.status) + " " + out_header.status_msg + "\n";
	if (out_header.connection != "")
		out += "Connection: " + out_header.connection + "\n";
	out += "Content-Length: " + itoa(out_body.length()) + "\n";
	out += "\n";
	out += out_body;
	package_length = out.length();
	return out;
}

void WebServ::respond_connection_from(int fd)
{
	ws_server_instance si;
	std::string raw_data;

	verbose(1) << "(webserv) Got connection from fd " << fd << "." << std::endl;
	raw_data = get_raw_data(fd);
	si = *fd_to_instance[fd];
	si.in_header = get_header(raw_data);
	si.in_body = get_body(raw_data);
	si.root_config.push_back("root", config.getValStr("working_directory"));
	verbose(1) << "(webserv) BODY >" << si.in_body << "<" << std::endl;

	ws_reply_instance respond(si);

	if (send(fd, respond.encapsulate().c_str(), respond.package_length, 0) == -1)
		throw std::domain_error("(webserv) Sending response went wrong.");
	remove_from_poll(fd);
}

void WebServ::light_up()
{
	int event;

	verbose(1) << "Light up server: " << config.getValStr("server_name") << std::endl;
	verbose(1) << config.getValStr("welcome_message") << std::endl;

	lit = true;
	while (lit)
	{
		event = catch_connection();
		if (event == 0) // stdin
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
	if (instance.empty())
		throw std::invalid_argument("(webserv) Configuration is empty.");
	hook_it();
	light_up();
	flush_stdin();
}

WebServ::WebServ(DataFold& u_config)
: config(u_config), server(config.filter("server")), lit(false)
{
	load_defaults();
	while (server.loop())
		instance.push_back(dftosi(server.val));
	return ;
}
