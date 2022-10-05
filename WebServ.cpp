/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 14:24:28 by fde-capu          #+#    #+#             */
/*   Updated: 2022/10/05 20:28:16 by fde-capu         ###   ########.fr       */
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

void WebServ::init()
{
	consider_stdin_because_grace();
	give_up_if_empty_configuration();
	hook_it();
	light_up();
}

void WebServ::consider_stdin_because_grace()
{ poll_list.push_back(stdin_to_pollfd()); }

void WebServ::give_up_if_empty_configuration()
{
	if (instance.empty())
		throw std::invalid_argument("(webserv) Configuration is empty.");
}

void WebServ::set_non_blocking(int sock)
{
	int opts;
	opts = O_NONBLOCK;
	if (fcntl(sock, F_SETFL, opts) == -1)
		throw std::domain_error("(webserv) Could not set non-blocking flag.");
}

int WebServ::bind_socket_to_local(int u_port)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp; // rp = read pointer
	int sfd, s;
	int yes = 1;

	hints = addrinfo();
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	s = getaddrinfo(NULL, itoa(u_port).c_str(), &hints, &result);
	if (s != 0)
		throw std::domain_error("(bind_socket_to_local) getaddrinfo failed: " + std::string(gai_strerror(s)));

	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
			throw std::domain_error("(bind_socket_to_local) Socket creation failed.");
		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
			throw std::domain_error("(bind_socket_to_local) Could not unlock the socket.");
		set_non_blocking(sfd);
		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
			break;
		close(sfd);
	}
	if (rp == NULL)
		throw std::domain_error("(bind_socket_to_local) Socket locked.");

	freeaddrinfo(result);
	verbose(2) << "(bind_socket_to_local) Bound fd " << sfd << " to port " << u_port << "." << std::endl;
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
				ufds.events = POLLIN; // | POLLOUT (check read and write same time..?)
				fd_to_port[ufds.fd] = instance[i].port[j];
				poll_list.push_back(ufds);
				if (listen(instance[i].listen_sock[j], SOMAXCONN) != 0)
					throw std::domain_error("(webserv) Listening problem.");
				taken_ports.push_back(instance[i].port[j]);
			}
			else if (!same_port_another_name(&instance[i]))
				verbose(2) << "(webserv) Warning: multiple servers configured on same port. Only the first will be used." << std::endl;
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
	set_non_blocking(newfd);
	poll_list.push_back(make_pollin_fd(newfd));
	fd_to_port[newfd] = fd_to_port[oldfd];

	verbose(2) << "(webserv) ======" << std::endl;
	verbose(2) << "(webserv) New connection from fd (" << oldfd << \
		")->" << newfd << "." << std::endl;
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

	verbose(2) << "(choose_instance) Responding as " << \
		choose->config.getValStr("server_name") << ":" << in_port << \
		"." << std::endl;

	return si;
}

void WebServ::respond_connection_from(int fd)
{
	int V(2);
	ws_server_instance si;
	std::string raw_data;
	std::string body;
	ws_header in_header;
	Chronometer time_out;
	CircularBuffer raw(fd);
	std::string encapsulated;

	verbose(1) << " . . . . . . . . . . . . . . . " << std::endl;
	verbose(V) << "(respond_connection_from) Getting data from fd " << fd \
		<< "." << std::endl;

	while (!in_header.is_valid)
	{
		if (time_out > TIME_OUT_MSEC)
		{
			verbose(2) << "(respond_connection_from) Timeout! << " << time_out << \
				" > Incomplete header" << std::endl;
			return respond_timeout(fd);
		}
		raw.receive_until("\r\n\r\n");
		in_header = get_header(raw.output);
	}

	verbose(V) << "(respond_connection_from) Got header:" << std::endl << \
		in_header << std::endl;

	si = choose_instance(in_header, fd_to_port[fd]);
	si.in_body = get_body(raw.receive_until_eof());
	si.set_props();
	si.set_sizes();
	si.fd = fd;
	ws_reply_instance respond(si); // ...oonn...
	respond.encapsulate();
	verbose(V) << "(respond_connection_from) respond.package_length " << respond.package_length << std::endl;
	verbose(V) << "(respond_connection_from) respond.out_body.length() " << respond.out_body.length() << std::endl;
	if (send(fd, respond.out_body.c_str(),
		respond.package_length, 0) == -1)
		throw std::domain_error("(webserv) Sending response went wrong.");
	close(fd);
	remove_from_poll(fd);
}

void WebServ::light_up()
{
	int V(2);
	int event;

	verbose(V) << "Light up server: " << \
		config.getValStr("server_name") << std::endl;
	verbose(CRITICAL) << config.getValStr("welcome_message") << std::endl;

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

WebServ::WebServ(DataFold& u_config)
: config(u_config), server(config.filter("server")), lit(false)
{
	load_defaults();
	while (server.loop())
		instance.push_back(dftosi(server.val));
	return ;
}
