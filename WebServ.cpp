/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 14:24:28 by fde-capu          #+#    #+#             */
/*   Updated: 2022/10/31 15:36:55 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

int WebServ::memuse(0);

WebServ::WebServ(DataFold& u_config)
: config(u_config), server(config.filter("server")), lit(false)
{
	buffer = static_cast<char *>(std::malloc(BUFFER_SIZE * sizeof(char) + 1));
	if (!buffer)
		throw std::domain_error("(webserv) Failed to initialize due to bad_alloc.");
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
	}
	catch(std::exception& e)
	{
		verbose(CRITICAL) << "(webserv) Error: " << e.what() << std::endl;
		throw e;
	}

	while (true)
	{
		try
		{
			light_up();
		}
		catch(std::exception& e)
		{
			verbose(CRITICAL) << "(webserv) Error: " << e.what() << std::endl;
		}
		if (!lit)
			break ;
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
				verbose(2) << "(webserv) Warning: multiple servers on same port. Using first." << std::endl;
		}
	}
	poll_list.push_back(stdin_to_pollfd()); // stdin exits gracefully.
	verbose(2) << "(webserv) I'm hooked." << std::endl << std::endl;
}

void WebServ::light_up()
{
	int V(3);
	struct pollfd event;
	std::map<int, std::pair<bool, bool> > ready;
	int client;
	int poll_count;

	verbose(V - 1) << "Light up server: " << \
		config.getValStr("server_name") << std::endl;
	if (!lit)
		verbose(V - 2) << config.getValStr("welcome_message") << std::endl;
	
	lit = true;
	while (lit) // Main loop.
	{
		std::cout << *this; // Animation.
		client = dispatch(ready);
		if (client)
			ready.erase(client);
		poll_count = poll(&poll_list[0], poll_list.size(), ZERO_TIMEOUT); // 1. Incomming connections.
		if (poll_count == -1)
			throw std::domain_error("(webserv) Poll error.");
		for (size_t i = 0; i < poll_list.size(); i++)
		{
			event = poll_list[i];
			if (poll_list[i].revents & POLLIN || poll_list[i].revents & POLLOUT) // Checks read and write at same time.
			{
				if (event.fd == -1)
					continue ;
				if (event.fd == 0)
					return exit_gracefully();
				if (event.revents & POLLIN)
				{
					if (is_a_webserv(event.fd))
					{
						verbose(2) << " . . . . . . . . . . . . . . . " << std::endl;
						dup_into_poll(event.fd);
						continue ;
					}
					if (!ready[event.fd].first)
						verbose(V + 1) << "(light_up) Got POLLIN from " << event.fd << std::endl;
					ready[event.fd].first = true;
					continue ;
				}
				if (event.revents & POLLOUT)
				{
					if (!ready[event.fd].second)
						verbose(V + 1) << "(light_up) Got POLLOUT from " << event.fd << std::endl;
					ready[event.fd].second = true;
					continue ;
				}
			}
			else if (poll_list[i].revents)
			{
				verbose(V + 2) << "(light_up) Some unknown event on " << poll_list[i].fd << std::endl;
			}
		}
	}
}

int WebServ::dispatch(std::map<int, std::pair<bool, bool> >& ready)
{
	int bytes;
	int fd;
	bool* pollin;
	bool* pollout;
	std::map<int, std::pair<bool, bool> >::iterator it;

	if_timout_mark_remove(ready);
	for (it = ready.begin(); it != ready.end(); it++)
	{
		TICK(ready.size() > 1 ? ready.size() / 2 : 0);
		fd = it->first;
		pollin = &(it->second.first);
		pollout = &(it->second.second);
		virgin[fd] = false;
		if (is_marked_to_remove(fd))
			return fd;
		if (*pollin)
		{
			bytes = read(fd, buffer, BUFFER_SIZE); // Reads from client.
			*pollin = false;
			if (bytes < 0) // -1
			{
				mark_to_remove(fd);
				continue ;
			}
			if (bytes == 0) // 0
				in_ended[fd] = true;
			if (bytes > 0)
			{
				if (above_memory_limits(fd, bytes))
					continue ;
				raw[fd].append(buffer, bytes);
				mem_usage_add(bytes);
				reset_chronometers(fd);
			}
			try_to_get_header(fd);
			try_to_choose_instance(fd);
			continue ;
		}

		check_in_finished(fd);
		check_if_body_ok(fd);
		choose_a_reply(fd);
		process_response(fd);

		if (*pollout)
		{
			if (encapsulated[fd])
			{
				*pollout = false;
				bytes = send(fd, respond[fd].out_body.c_str(), respond[fd].package_length, 0); // Writes to client.
				if (bytes < 0) // -1
				{
					mark_to_remove(fd);
					continue ;
				}
				if (bytes == 0) {} // 0
				if (bytes > 0)
				{
					unload_body(fd, bytes);
					mem_usage_substract(bytes);
					reset_chronometers(fd);
				}
				check_for_anything_left(fd);
			}
			continue ;
		}
	}
	return 0;
}

ws_reply_instance::ws_reply_instance(ws_server_instance& si)
{
	int V(5);
	verbose(V) << "(ws_reply_instance) Constructor." << std::endl;
	init();

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
	if (is_201(si)) return ; // POST and saves data.
	if (is_200(si)) return ; // Ok GET and loads file.

	set_code(420, "Enhance Your Calm");
	template_page(420, custom_error(420, si));
}

ws_server_instance WebServ::choose_instance(ws_header& in, int in_port)
{
	int V(3);
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
				if (instance[i].config.getValStr("server_name") == in.host)
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
	if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1)
		throw std::domain_error("(webserv) Could not set non-blocking flag.");
}

void WebServ::remove_from_poll(int fd)
{
	for (size_t i = 0; i < poll_list.size(); i++)
	{
		if (poll_list[i].fd == fd)
		{
			std::vector<struct pollfd>::iterator position(&poll_list[i]);
			if (position != poll_list.end())
				poll_list.erase(position);
			break ;
		}
	}
}

void WebServ::dup_into_poll(int oldfd)
{
	int V(2);
	struct sockaddr_storage remoteaddr;
	unsigned int addrlen = sizeof remoteaddr;
	int newfd = accept(oldfd, (struct sockaddr *)&remoteaddr, &addrlen);
	if (newfd == -1)
		throw std::domain_error("(webserv) Unacceptable connection.");
	set_non_blocking(newfd);
	poll_list.push_back(make_in_out_fd(newfd));
	fd_to_port[newfd] = fd_to_port[oldfd];
	timer[newfd].btn_reset();
	virgin[newfd] = true;

	verbose(V) << "(webserv) Connection " << poll_list.size() << \
		" from fd (" << oldfd << ")->" << newfd << "." << std::endl;
}
