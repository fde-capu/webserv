/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ_works.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/21 17:57:36 by fde-capu          #+#    #+#             */
/*   Updated: 2022/10/24 16:22:26 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

bool ws_reply_instance::is_working_cgi(ws_server_instance& si)
{
	int V(1);
	bool action_dump(false);
	bool action_get(false);

	if (!si.is_cgi() \
		|| out_header.status == 301 \
		|| out_header.status == 403 \
		|| out_header.status == 404 \
	)
		return false;
	if (dumping_to_cgi)
	{
		action_dump = cgi_dumping(si);
		if (!action_dump)
		{
			close(pipe_pc[1]);
			dumping_to_cgi = false;
		}
	}
	if (getting_from_cgi)
	{
		action_get = cgi_receiving();
		if (!action_get)
		{
			getting_from_cgi = false;
			wait(0);
			//		std::vector<struct pollfd>::iterator position(&poll_list[pipe_cp[0]]);
			//		poll_list.erase(position);
			free(buffer);
			close(pipe_cp[0]);
			verbose(V) << "(is_working_cgi) Got >>>" << LONG(out_body) << "<<<" << std::endl;
			header_from_body();
			if (si.in_header.is_post())
			{
				set_code(202, "Accepted");
				return false;
			}
			else
			{
				set_code(200, "OK");
				return false;
			}
		}
	}
	return action_dump || action_get;
}

bool ws_reply_instance::is_working_load(ws_server_instance& si)
{
	int V(1);

	if (!to_work_load || si.is_cgi())
		return false;
	verbose(V) << "(is_working_load) TO DO ASYNC" << std::endl;
	out_body = FileString(file_name.c_str()).content();
	to_work_load = false;
	return false;
}

bool ws_reply_instance::is_working_save(ws_server_instance& si)
{
	int V(1);
	std::string* data;
	int poll_count;
	int TIME_OUT = 0; // non-blocking.
	int sbytes;
	size_t wr_size;

	if (!si.in_header.is_post() || save_canceled() || si.is_cgi())
		return false;
	if (si.is_chunked() && !si.chunk_finished)
	{
		si.mount_chunked();
		return true;
	}
	if (si.is_multipart() && !si.multipart_finished)
	{
		si.mount_multipart();
		return true;
	}
	if (si.is_multipart())
		data = &si.multipart_content;
	else if (si.is_chunked())
		data = &si.chunked_content;
	else
		data = &si.in_body;
	
	verbose(CRITICAL) << "(webserv) >" << SHORT((*data)) << \
		"< will be saved into " << full_path << \
		"." << std::endl;

	poll_count = poll(&poll_list[0], poll_list.size(), TIME_OUT);
	if (poll_count == -1)
		throw std::domain_error("(webserv) Poll error.");
	for (size_t i = 0; i < poll_list.size(); i++)
	{
		if (!poll_list[i].revents)
			continue ;
		if (poll_list[i].revents & POLLOUT)
		{
			wr_size = data->length() > ASYNC_CHUNK_SIZE ? ASYNC_CHUNK_SIZE : data->length();
			verbose(V) << "(is_working_save) Writing into " << poll_list[i].fd << std::endl;
			sbytes = write(poll_list[i].fd, data->c_str(), wr_size);
			if (sbytes < 0)
				return false;
			if (sbytes > 0)
				StringTools::consume_bytes(*data, sbytes);
			if (si.is_multipart())
				data = &si.multipart_content;
			else if (si.is_chunked())
				data = &si.chunked_content;
			else
				data = &si.in_body;
			verbose(V) << " - Saved " << sbytes << ", " << data->length() << " left." << std::endl;
			if (data->length() == 0)
			{
				verbose(V) << poll_list[i].fd << " - Finished saving (removed)." << std::endl;
				std::vector<struct pollfd>::iterator position(&poll_list[i]);
				poll_list.erase(position);
				return false;
			}
		}
	}
	return true;
}
