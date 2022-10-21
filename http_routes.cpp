/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_routes.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/29 15:31:47 by fde-capu          #+#    #+#             */
/*   Updated: 2022/10/21 16:14:14 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
extern DataFold g_config;

int ws_reply_instance::PUT_mock(ws_server_instance& si)
{
	if (si.in_header.method == "PUT")
	{
		set_code(200, "OK, but PUT is mocked.");
		out_body = "PUT is MOCKED, because it is not mandatory for this project.";
		return 200;
	}
	return 0;
}

int ws_reply_instance::is_501(ws_server_instance& si)
{
	if (
		(si.in_header.method == "PUT")
	)
	{
		set_code(501, "Not Implemented");
		out_body = TemplatePage::page(501, si.custom_error(501));
		return 501;
	}
	return 0;
}

int ws_reply_instance::is_301(ws_server_instance& si)
{
	std::vector<std::string> returns;
	returns = si.config.get_vector_str("return");
	if (!returns.empty())
	{
		if (atoi(returns[0].c_str()) == 301)
		{
			set_code(301, "Moved Permanently");
			out_body = TemplatePage::page(301);
			set_redirect(returns[1]);
			return 301;
		}
	}
	return 0;
}

int ws_reply_instance::is_405(ws_server_instance& si)
{
	bool method_accepted(false);

	DataFold accepted_methods = si.server_location_config(
		"accepted_request_methods",
		DEFAULT_ACCEPTED_METHODS
	);
	while (accepted_methods.loop())
	{
		if (si.in_header.method == accepted_methods.val)
			method_accepted = true;
	}
	if (method_accepted)
		return 0;
	set_code(405, "Method Not Allowed");
	out_body = TemplatePage::page(405, si.custom_error(405));
	return 405;
}

int ws_reply_instance::is_403(ws_server_instance& si)
{
	DataFold locations(si.config.get<DataFold>("location"));

	if (	(locations.empty() && si.val("root") == "")
		||	(si.in_header.method == "DELETE" && FileString::is_dir(si.location_path()))
		)
	{
		set_code(403, "Forbidden");
		out_body = TemplatePage::page(403, si.custom_error(403));
		return 403;
	}
	return 0;
}

int ws_reply_instance::is_404(ws_server_instance& si)
{
	int V(3);
	std::string request;
	DataFold indexes;
	std::string \
		autoindex(si.server_location_config("autoindex").getValStr("autoindex"));

	request = si.location_path();

	verbose(V) << "(is_404) request: " << request << std::endl;

	if (si.in_header.method == "GET")
	{
		if (FileString::is_dir(request))
		{
			indexes = si.server_location_config("index");
			while (indexes.loop())
			{
				file_name = si.location_path(indexes.val);
				verbose(V) << "(webserv) Fetching " << file_name \
					<< std::endl;
				if (FileString::exists(file_name))
					return 0;
			}
		}
		else
		{
			if (FileString::exists(request))
				return 0;
		}
		if (FileString::is_dir(request))
		{
			if (autoindex == "on")
				return list_autoindex(request, si);
			else if (autoindex != "")
			{
				set_code(403, "Forbidden");
				out_body = TemplatePage::page(403, si.custom_error(403));
				return 403;
			}
		}
		verbose(V) << "(is_404) autoindex: " << autoindex << std::endl;
		set_code(404, "File Not Found");
		out_body = TemplatePage::page(404, si.custom_error(404));
		return 404;
	}
	if (si.in_header.method == "DELETE")
	{
		if (FileString::exists(request))
			return 0;
		set_code(404, "File Not Found");
		out_body = TemplatePage::page(404, si.custom_error(404));
		return 404;
	}
	return 0;
}

int ws_reply_instance::is_413_507_422(ws_server_instance& si)
{
	static int V(1);
	int pos_status(0);

	verbose(V) << "(is_413_507_422) max_size: " << si.max_size << "." \
		<< std::endl;
	verbose(V) << "(is_413_507_422) content_type: " << \
		si.in_header.content_type << std::endl;

	pos_status = si.process_post();
	if (si.exceeded_limit)
	{
		set_code(413, "Payload Too Large");
		out_body = TemplatePage::page(413, si.custom_error(413));
		return 413;
	}
	if (si.in_header.content_length > MEMORY_LIMIT)
	{
		set_code(507, "Insufficient Resources");
		out_body = TemplatePage::page(507, si.custom_error(507));
		return 507;
	}
	if (pos_status == 422)
	{
		set_code(422, "Unprocessable Entity");
		out_body = TemplatePage::page(422, si.custom_error(422));
		return 422;
	}

	verbose(V) << "(is_413_507_422) in_body >>" << SHORT(si.in_body) << "<<" << std::endl;
	verbose(V) << "(is_413_507_422) multipart_content >>" << SHORT(si.multipart_content) << "<<" << std::endl;
	verbose(V) << "(is_413_507_422) chunked_content >>" << SHORT(si.chunked_content) << "<<" << std::endl;

	return 0;
}

int ws_reply_instance::is_424(ws_server_instance& si)
{
	if (is_equal_insensitive(si.in_header.expect, "100-continue"))
	{
		verbose(2) << "(is_424) Because of " << si.in_header.expect << \
			", and sizes are ok, would return 100-continue. " \
			"webserv must always close connection, so, instead, " \
			"will return 424 Failed Dependency." << std::endl;

		set_code(424, "Failed Dependency");
		out_body = TemplatePage::page(424, si.custom_error(424));
		return 424;
	}
	return 0;
}

int ws_reply_instance::is_400(ws_server_instance& si)
{
	std::string full_path = si.location_path(si.multipart_filename);
	if (si.is_chunked() && FileString::is_dir(full_path))
	{
		set_code(400, "Bad Request");
		out_body = TemplatePage::page(400);
		return 400;
	}
	return 0;
}

int ws_reply_instance::is_201(ws_server_instance& si)
{
	int V(1);

	if (si.in_header.is_post() && !save_canceled())
	{
		full_path = si.location_path(si.multipart_filename);
		verbose(V) << "(is_201) Opening " << full_path << "." << std::endl;
		file_fd = open(full_path.c_str(), O_WRONLY | O_CREAT | O_NONBLOCK, S_IRUSR | S_IWUSR);
		if (file_fd == -1)
			throw std::domain_error("(webserv) Cannot open file to save data.");
		if (fcntl(file_fd, F_SETFL, O_NONBLOCK) == -1)
			throw std::domain_error("(webserv) Could not set non-blocking file.");
		verbose(V) << "(is_201) as " << file_fd << "." << std::endl;
		poll_list.push_back(WebServ::make_in_out_fd(file_fd));
		set_code(201, "Created");
		out_body = TemplatePage::page(201);
		return 201;
	}
	return 0;
}

bool ws_reply_instance::is_working_cgi(ws_server_instance& si)
{
	int V(1);

	if (!si.is_cgi() \
		|| out_header.status == 301 \
		|| out_header.status == 403 \
		|| out_header.status == 404 \
	)
		return false;
	if (dumping_to_cgi)
	{
		if (cgi_dumping(si))
			return true;
		close(pipe_pc[1]);
		dumping_to_cgi = false;
		dumping_to_cgi_finished = true;
	}
	if (getting_from_cgi && dumping_to_cgi_finished)
	{
		if (cgi_receiving())
			return true;
		getting_from_cgi = false;
		wait(0);
//		std::vector<struct pollfd>::iterator position(&poll_list[pipe_cp[0]]);
//		poll_list.erase(position);
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
	return true;
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
	size_t ASYNC_CHUNK_SIZE(10000000);
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

int ws_reply_instance::is_200(ws_server_instance& si)
{
	int V(2);
	std::string request;
	DataFold indexes;

	request = si.location_path();
	verbose(V) << "(is_200) request: " << si.in_header.method << " " << request << std::endl;
	if (si.in_header.method == "GET")
	{
		if (FileString::is_dir(request))
		{
			indexes = si.server_location_config("index");
			while (indexes.loop())
			{
				file_name = si.location_path(indexes.val);
				if (FileString::exists(file_name))
					break ;
			}
		}
		else
			file_name = request;
		to_work_load = true;
		set_code(200, "OK");
		verbose(V) << "(is_working_load) out_body" << std::endl << SHORT(out_body) << std::endl;
		return 200;
	}
	if (si.in_header.method == "DELETE")
	{
		verbose(V) << "(is_200) will delete " << request << std::endl;
		if (std::remove(request.c_str()) != 0)
		{
			set_code(422, "Unprocessable Entity");
			out_body = TemplatePage::page(422, si.custom_error(422));
			return 422;
		}
		else
		{
			set_code(200, "OK");
			out_body = TemplatePage::page(200);
			return 200;
		}
	}
	return 0;
}
