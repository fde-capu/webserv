/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiWrapper.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/26 14:07:52 by fde-capu          #+#    #+#             */
/*   Updated: 2022/06/22 12:24:40 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiWrapper.hpp"

CgiWrapper::CgiWrapper(std::string u_executable, int u_port)
: executable(u_executable), port(u_port)
{
	verbose(1) << "Executable: " << executable << std::endl;
	verbose(1) << "Port: " << port << std::endl;

	struct pollfd ufds;
	int poll_count;
	int TIME_OUT = 0; // 0 = non-blocking, -1 = blocking, N = cycle blocking ms
	int event;
	struct sockaddr_storage remoteaddr;
	unsigned int addrlen = sizeof remoteaddr;
	int newfd;
	std::string raw_data;
	std::string body;

	// Bind
	try {
		listen_sock = WebServ::bind_socket_to_local(port);
	} catch (std::exception&e) {
		throw std::domain_error("(CgiWrapper borrowed): " + std::string(e.what()));
	}

	// Listen
	ufds = pollfd();
	ufds.fd = listen_sock;
	ufds.events = POLLIN;
	poll_list.push_back(ufds);
	if (listen(listen_sock, SOMAXCONN) != 0)
		throw std::domain_error("(CgiWrapper) Listening went wrong.");
	verbose(1) << "^> (CgiWrapper) I'm listening." << std::endl;

do_it_again:

	// Catch connection.
	event = 0;
	while (!event)
	{
		poll_count = poll(&poll_list[0], poll_list.size(), TIME_OUT);
		if (poll_count == -1)
			throw std::domain_error("(CgiWrapper) Poll crashed.");
		for (size_t i = 0; i < poll_list.size(); i++)
		{
			if (poll_list[i].revents & POLLIN)
			{
				event = poll_list[i].fd;
				break ;
			}
		}
	}

	// Answer the phone.
	newfd = accept(event, (struct sockaddr *)&remoteaddr, &addrlen);
	if (newfd == -1)
		throw std::domain_error("(CgiWrapper) Could not accept call.");
	ufds.fd = newfd;
	poll_list.push_back(ufds);
	verbose(1) << "(CgiWrapper) Received call (" << event << ")->" << newfd << "." << std::endl;

	// Read data.
	raw_data = CircularBuffer(newfd);
	verbose(5) << "[RAW>>" << raw_data << "<<RAW]" << std::endl;

	body = WebServ::get_body(raw_data);
	verbose(5) << "[BODY>>" << body << "<<BODY]" << std::endl;

//	// Parse content.
//	ws_server_instance si;
//	si.in_header = WebServ::get_header(raw_data);
//	if (!si.in_header.is_valid)
//	{
//		remove_from_poll(newfd);
//		goto do_it_again;
//	}
//	si.in_body = WebServ::get_body(raw_data);

//	// Check content for validity..? Sure thing.
//	(void)void*foo; // Why did it parse, then? Why did it even read?

//	// Make up some response.
	ws_cgi_reply body_from (executable, body);
	verbose(5) << "[CGI>>" << body_from.out_body << "<<CGI]" << std::endl;
//	if (send(newfd, body_from.out_body.c_str(), body_from.out_body.length(), 0) == -1)
	if (send(newfd, body_from.encapsulate().c_str(), body_from.package_length, 0) == -1)
		throw std::domain_error("(CgiWrapper) Could not send response.");
	close(newfd);
	verbose(1) << "(CgiWrapper) Closed call " << newfd << "." << std::endl;
	remove_from_poll(newfd);
	goto do_it_again;
}

ws_cgi_reply::ws_cgi_reply(std::string& exec_cgi, std::string& raw_data)
{
	out_body = "[:(] ";

	out_header.method = "";
	out_header.directory = "/";
	out_header.protocol = "HTTP/1.1";
	out_header.status = 200;
	out_header.status_msg = "OK";
	out_header.connection = "close";

	pid_t child_pid = -1;
	pid_t wait_pid = -1;
	int child_status = -1;
	char* args[2] = {0, 0};
	int pipe_pc[2] = {0, 0};
	int pipe_cp[2] = {0, 0};

	if (pipe(pipe_pc) == -1)
		throw std::domain_error("(webserv) Cannot pipe for cgi (parent->child).");
	if (pipe(pipe_cp) == -1)
		throw std::domain_error("(webserv) Cannot pipe for cgi (child->parent).");

	child_pid = fork();
	if (child_pid < 0)
		throw std::domain_error("(webserv) Forking went wrong.");
	if (child_pid == 0) // Child.
	{
		dup2(pipe_pc[0], STDIN_FILENO);
		dup2(pipe_cp[1], STDOUT_FILENO);
		close(pipe_pc[0]);
		close(pipe_pc[1]);
		close(pipe_cp[0]);
		close(pipe_cp[1]);
		args[0] = (char *)exec_cgi.c_str();
		execvp(exec_cgi.c_str(), args);
		exit(502);
	}
	else // Parent.
	{
		write(pipe_pc[1], raw_data.c_str(), raw_data.length());

//(void)raw_data;
//		fflush(stdout);

		close(pipe_pc[0]);
		close(pipe_pc[1]);
		close(pipe_cp[1]);

		wait_pid = wait(&child_status);
		if (wait_pid < 0)
			throw std::domain_error("(webserv) Coudn't wait.");
		out_body = CircularBuffer(pipe_cp[0]);
		verbose(5) << ">>>" << out_body << "<<<" << std::endl;
		static_cast<void>(WIFEXITED(child_status));
	}
}

void CgiWrapper::remove_from_poll(int fd)
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
	throw std::domain_error("(CgiWrapper) Can not remove fd from poll.");
}

std::ostream & operator<< (std::ostream & o, CgiWrapper const & self)
{
	o << "::CgiWrapper::" << std::endl;
	static_cast<void>(self);
	return o;
}

CgiWrapper::~CgiWrapper()
{
	return ;
}
