/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ_dispatches.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/28 18:58:39 by fde-capu          #+#    #+#             */
/*   Updated: 2022/11/02 22:32:59 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

void WebServ::if_timout_mark_remove(std::map<int, std::pair<bool, bool> >& ready)
{
	int V(3);
	int fd;

	for (std::map<int, Chronometer>::iterator it = timer.begin(); \
		it != timer.end(); it++)
	{
		fd = it->first;
		if (timer[fd] > ACTIVITY_TIMEOUT && !virgin[fd])
		{
			verbose(V) << "(dispatch) Marked to remove by timeout " << fd \
				<< std::endl;
			ready[fd].first = true;
			remove_client[fd] = true;
		}
	}
}

bool WebServ::is_marked_to_remove(int fd)
{
	int V(3);

	if (remove_client[fd])
	{
		verbose(V) << fd << " :: " << respond[fd].out_header.status << " :: " << respond[fd].out_header.status_msg << std::endl;
		verbose(V) << "(dispatch) Closing fd " << fd << std::endl;
		close(fd);
		mem_usage_substract(respond[fd].out_body.length());
		respond.erase(fd);
		remove_from_poll(fd);
		webserver.erase(fd);
		in_ended.erase(fd);
		out_ended.erase(fd);
		body_ok.erase(fd);
		chosen_instance.erase(fd);
		encapsulated.erase(fd);
		mem_usage_substract(raw[fd].length());
		raw.erase(fd);
		in_header.erase(fd);
		remove_client.erase(fd);
		response_working.erase(fd);
		chosen_response.erase(fd);
		timer.erase(fd);
		virgin.erase(fd);
		verbose(V) << fd << " - Removed." << std::endl;
		return true;
	}
	return false;
}

void WebServ::mark_to_remove(int fd)
{
	int V(3);

	verbose(V) << "(webserv) Marked to remove by I/O fail " << fd << std::endl;
	remove_client[fd] = true;
}

bool WebServ::above_memory_limits(int fd, int bytes)
{
	return \
		raw[fd].length() + bytes > MEMORY_LIMIT \
	||	WebServ::memuse + bytes > SAFE_LIMIT;
}

void WebServ::mem_usage_add(int bytes)
{
	int V(4);

	memuse += bytes;
	verbose(V) << " * memuse += " << bytes << " (" << WebServ::memuse << ")" << std::endl;
}

void WebServ::mem_usage_substract(int bytes)
{
	int V(4);

	memuse -= bytes;
	if (memuse < 0)
		memuse = 0;
	verbose(V) << " * memuse -= " << bytes << " (" << WebServ::memuse << ")" << std::endl;
}

void WebServ::reset_chronometers(int fd)
{
	webserver[fd].chronometer.btn_reset();
	timer[fd].btn_reset();
}

void WebServ::choose_instance(int fd)
{
	int V(2);

	verbose(V) << fd << " - Got header." << std::endl;
	webserver[fd] = choose_instance(in_header[fd], fd_to_port[fd]);
	verbose(V) << fd << " - Has instance." << std::endl;
	webserver[fd].chronometer.btn_reset();
	timer[fd].btn_reset();
	chosen_instance[fd] = true;
}

void WebServ::check_in_finished(int fd)
{
	if (!in_ended[fd] && chosen_instance[fd] \
			&& webserver[fd].chronometer > INCOME_TIMEOUT)
		in_ended[fd] = true;
}

void WebServ::check_if_body_ok(int fd)
{
	int V(2);

	if (in_ended[fd] && !body_ok[fd])
	{
		webserver[fd].in_body = get_body(raw[fd]);
		verbose(V) << fd << " - In body mounted." << std::endl;
		webserver[fd].set_props();
		webserver[fd].set_sizes();
		webserver[fd].fd = fd;
		body_ok[fd] = true;
	}
}

void WebServ::choose_a_reply(int fd)
{
	int V(2);

	if (!chosen_response[fd] && in_ended[fd])
	{
		verbose(V) << fd << " - Will reply." << std::endl;
		respond[fd] = ws_reply_instance(webserver[fd]); // ...oonn...
		chosen_response[fd] = true;
		verbose(V) << fd << " - Chosen reply." << std::endl;
		response_working[fd] = true;
	}
}

void WebServ::process_response(int fd)
{
	int V(2);

	if (chosen_response[fd] && respond[fd].to_work_load)
		response_working[fd] = true;
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
}

void WebServ::unload_body(int fd, int& bytes)
{
	if (bytes > static_cast<int>(respond[fd].out_body.length()))
		bytes = respond[fd].out_body.length();
	StringTools::consume_bytes(respond[fd].out_body, bytes);
}

void WebServ::try_to_get_header(int fd)
{
	if (!in_header[fd].is_valid)
		in_header[fd] = get_header(raw[fd]);
}

void WebServ::try_to_choose_instance(int fd)
{
	if (in_header[fd].is_valid && !chosen_instance[fd])
		choose_instance(fd);
}

void WebServ::check_for_anything_left(int fd)
{
	int V(3);

	out_ended[fd] = respond[fd].out_body.length() == 0;
	remove_client[fd] = out_ended[fd];
	if (remove_client[fd])
		verbose(V) << "(dispatch) Marked to remove by nothing left " << fd << std::endl;
}
