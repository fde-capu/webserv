/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 14:24:28 by fde-capu          #+#    #+#             */
/*   Updated: 2022/10/27 00:34:45 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

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
	int V(2);
	struct pollfd event;
	std::map<int, std::pair<bool, bool> > ready;
	int client;
	int TIME_OUT = 0; // 0: non-blocking, -1: blocking, N: cycle blocking ms
	int poll_count;

	verbose(V) << "Light up server: " << \
		config.getValStr("server_name") << std::endl;
	if (!lit)
		verbose(CRITICAL) << config.getValStr("welcome_message") << std::endl;

	lit = true;
	while (lit) // Main loop.
	{
		std::cout << *this; // Animation.
		client = dispatch(ready);
		if (client)
			ready.erase(client);

		poll_count = poll(&poll_list[0], poll_list.size(), TIME_OUT); // 1. Incomming connections.
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
						verbose(V) << " . . . . . . . . . . . . . . . " << std::endl;
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
				verbose(V) << "(catch_connection) Some unknown event on " << poll_list[i].fd << std::endl;
			}
		}
	}
}

int WebServ::dispatch(std::map<int, std::pair<bool, bool> >& ready)
{
	int V(2);
	int INCOME_TIMEOUT(50);
	int ACTIVITY_TIMEOUT(2000);
	int rbytes;
	int sbytes;
	int fd;
	bool* pollin;
	bool* pollout;
	std::map<int, std::pair<bool, bool> >::iterator it;

	for (std::map<int, Chronometer>::iterator it = timer.begin(); it != timer.end(); it++)
	{
		fd = it->first;
		if (timer[fd] > ACTIVITY_TIMEOUT && !virgin[fd])
		{
			verbose(V + 1) << "(dispatch) Marked to remove by timeout " << fd << std::endl;
			ready[fd].first = true;
			remove_client[fd] = true;
		}
	}

	for (it = ready.begin(); it != ready.end(); it++)
	{
		fd = it->first;
		pollin = &(it->second.first);
		pollout = &(it->second.second);
		virgin[fd] = false;

		if (remove_client[fd])
		{
			verbose(V + 1) << "(dispatch) Closing fd " << fd << std::endl;
			close(fd);
			respond.erase(fd);
			remove_from_poll(fd);
			webserver.erase(fd);
			in_ended.erase(fd);
			out_ended.erase(fd);
			body_ok.erase(fd);
			chosen_instance.erase(fd);
			encapsulated.erase(fd);
			raw.erase(fd);
			in_header.erase(fd);
			remove_client.erase(fd);
			response_working.erase(fd);
			chosen_response.erase(fd);
			timer.erase(fd);
			virgin.erase(fd);
			verbose(V) << fd << " - Removed." << std::endl;
			return fd;
		}

		if (*pollin)
		{
			*pollin = false;
			rbytes = read(fd, buffer, BUFFER_SIZE); // Reads from client.
			if (rbytes < 0) // -1
			{
				verbose(V + 1) << "(webserv) Marked to remove by read fail " << fd << std::endl;
				remove_client[fd] = true;
				continue ;
			}
			if (rbytes == 0) // 0
				in_ended[fd] = true;
			if (rbytes > 0)
			{
				if (raw[fd].length() + rbytes > MEMORY_LIMIT)
					continue ;
				raw[fd].append(buffer, rbytes);
				webserver[fd].chronometer.btn_reset();
				timer[fd].btn_reset();
			}
			if (!in_header[fd].is_valid)
				in_header[fd] = get_header(raw[fd]);
			if (in_header[fd].is_valid && !chosen_instance[fd])
			{
				verbose(V) << fd << " - Got header." << std::endl;
				webserver[fd] = choose_instance(in_header[fd], fd_to_port[fd]);
				chosen_instance[fd] = true;
				verbose(V) << fd << " - Has instance." << std::endl;
				webserver[fd].chronometer.btn_reset();
				timer[fd].btn_reset();
			}
			continue ;
		}

		if (!in_ended[fd] && chosen_instance[fd] \
			&& webserver[fd].chronometer > INCOME_TIMEOUT)
			in_ended[fd] = true;
		if (in_ended[fd] && !body_ok[fd])
		{
			webserver[fd].in_body = get_body(raw[fd]);
			verbose(V) << fd << " - In body mounted." << std::endl;
			webserver[fd].set_props();
			webserver[fd].set_sizes();
			webserver[fd].fd = fd;
			body_ok[fd] = true;
		}
		if (!chosen_response[fd] && in_ended[fd])
		{
			verbose(V) << fd << " - Will reply." << std::endl;
			respond[fd] = ws_reply_instance(webserver[fd]); // ...oonn...
			chosen_response[fd] = true;
			verbose(V) << fd << " - Chosen reply." << std::endl;
			response_working[fd] = true;
		}
		if (chosen_response[fd] && respond[fd].to_work_load)
		{
			response_working[fd] = true;
		}
		if (response_working[fd])
		{
			bool l(respond[fd].is_working_load(webserver[fd]));
			bool s(respond[fd].is_working_save(webserver[fd]));
			bool c(respond[fd].is_working_cgi(webserver[fd]));
			if (l || s || c)
				timer[fd].btn_reset();
			response_working[fd] = l || s || c;
		}
		if (chosen_response[fd] && !response_working[fd] && !encapsulated[fd])
		{
			respond[fd].encapsulate();
			encapsulated[fd] = true;
			verbose(V) << fd << " - Response encapsulated." << std::endl;
		}

		if (*pollout)
		{
			if (encapsulated[fd])
			{
				*pollout = false;
				sbytes = send(fd, respond[fd].out_body.c_str(), respond[fd].package_length, 0); // Writes to client.
				if (sbytes < 0) // -1
				{
					verbose(V + 1) << "(webserv) Marked to remove by send fail " << fd << std::endl;
					remove_client[fd] = true;
					continue ;
				}
				if (sbytes == 0) {} // 0
				if (sbytes > 0)
				{
					StringTools::consume_bytes(respond[fd].out_body, sbytes);
					webserver[fd].chronometer.btn_reset();
					timer[fd].btn_reset();
				}
				verbose(V + 1) << fd << " - Sent " << sbytes << ", " << respond[fd].out_body.length() << " left." << std::endl;
				out_ended[fd] = respond[fd].out_body.length() == 0;
				remove_client[fd] = out_ended[fd];
				if (remove_client[fd])
					verbose(V + 1) << "(dispatch) Marked to remove by nothing left " << fd << std::endl;
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

	verbose(V) << "(webserv) Connection from fd (" << oldfd << \
		")->" << newfd << "." << std::endl;
}
