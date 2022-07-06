/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_routes.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/29 15:31:47 by fde-capu          #+#    #+#             */
/*   Updated: 2022/07/06 16:48:26 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

int ws_reply_instance::is_301(ws_server_instance& si)
{
	std::vector<std::string> returns;
	returns = si.config.get_vector_str("return");
	if (!returns.empty())
	{
		if (atoi(returns[0].c_str()) == 301)
		{
			set_code(301, "Moved Permanently");
			set_redirect(returns[1]);
			return 301;
		}
	}
	return 0;
}

int ws_reply_instance::is_403(ws_server_instance& si)
{
	DataFold locations(si.config.get<DataFold>("location"));

	if (locations.empty() && si.val("root") == "")
	{
		set_code(403, "Forbidden");
		return 403;
	}
	return 0;
}

int ws_reply_instance::is_405(ws_server_instance& si)
{
	DataFold accepted_methods(
		si.config.getValStr("accepted_request_methods") != "" ?
			split(si.config.getValStr("accepted_request_methods"), " ") :
			split(std::string(DEFAULT_ACCEPTED_METHODS), " ")
	);
	bool method_accepted(false);
	DataFold locations(si.config.get<DataFold>("location"));
	DataFold loc;

	if (!locations.empty())
	{
		while (locations.loop())
		{
			loc = locations.val;
			if (loc.getValStr("uri") == si.in_header.directory)
				while (loc.loop())
					if (loc.key == "accepted_request_methods")
						accepted_methods = loc.get("accepted_request_methods");
		}
	}
	while (accepted_methods.loop())
		if (si.in_header.method == accepted_methods.val)
			method_accepted = true;
	if (method_accepted) return 0;
	set_code(405, "Method Not Allowed");
	return 405;
}

int ws_reply_instance::is_404(ws_server_instance& si)
{
	DataFold indexes;
	std::string file_name;
//	DataFold locations(si.config.get<DataFold>("location"));

	if (si.in_header.method == "GET")
	{
		indexes = si.config.get("index");
		while (indexes.loop())
		{
			file_name = si.root_config.getValStr("root") \
						+ "/" + si.val("root") \
						+ si.in_header.directory \
						+ "/" + indexes.val;
			//stool.remove_rep_char(file_name, '/'); // The kernel doesn't really care.
			verbose(3) << "(webserv) Fetching " << file_name << std::endl;
			FileString from_file(file_name.c_str());
			out_body = from_file.content();
			if (out_body != "")
				return 0;
		}
		set_code(404, "File Not Found");
		verbose(1) << "(webserv) ! 404 " << file_name \
			<< std::endl;
		return 404;
	}
	return 0;
}

int ws_reply_instance::is_200(ws_server_instance& si)
{
	DataFold indexes;
	std::string file_name;
//	DataFold locations(si.config.get<DataFold>("location"));

	if (si.in_header.method == "GET")
	{
		indexes = si.config.get("index");
		while (indexes.loop())
		{
			file_name = si.root_config.getValStr("root") \
						+ "/" + si.val("root") \
						+ si.in_header.directory \
						+ "/" + indexes.val;
			// stool.remove_rep_char(file_name, '/'); // The kernel doesn't really care.
			verbose(3) << "(webserv) Fetching " << file_name << std::endl;
			FileString from_file(file_name.c_str());
			out_body = from_file.content();
			if (out_body != "")
			{
				set_code(200, "OK");
				return 200;
			}
		}
	}
	return 0;
}

DataFold ws_reply_instance::get_location_config(ws_server_instance& si)
{
	DataFold locations(si.config.get<DataFold>("location"));
	if (locations.empty()) return locations;
	while (locations.loop())
		if (DataFold(locations.val).getValStr("uri") == si.in_header.directory)
			return locations.val;
	return locations;
}

void ws_server_instance::boundary_start_end(size_t& start, size_t& end)
{
	start = in_body.find(boundary);
	if (start == std::string::npos)
		start = 0;
	else
		start = in_body.find("\r\n", start + boundary.length()) + 2;
	end = in_body.find(boundary, start);
	end = end == std::string::npos ? in_body.length() : end - 5;
}

void ws_server_instance::body_start_end(const size_t& start, const \
	size_t& end, size_t& body_start, size_t& body_end)
{
	body_start = in_body.find("\r\n\r\n", start);
	body_start += body_start == std::string::npos ? 0 : 4;
	body_end = end;
}

std::string ws_server_instance::read_fd_for_boundary_at_most()
{
	std::string dir_name(in_header.directory);
	size_t len(in_header.content_length);
	size_t start, end, body_start, body_end;

	verbose(1) << "(read_fd_for_boundary_at_most) " << dir_name << \
		" accepting at most " << max_size << " bytes." << std::endl;
	verbose(1) << "(read_fd_for_boundary_at_most) Will read " << len << \
		" bytes for " << multipart_type << " using boundary >>" << boundary \
		<< "<<." << std::endl;

	boundary_start_end(start, end);
	body_start_end(start, end, body_start, body_end);

	verbose(1) << "(read_fd_for_boundary_at_most) Start: " << start << \
		", end: " << end << "." << std::endl;
	verbose(1) << "(read_fd_for_boundary_at_most) Body start: " << \
		body_start << ", end: " << body_end << "." << std::endl;

	std::string body_block;
	body_block = in_body.substr(body_start, body_end - body_start);
	verbose(1) << "(read_fd_for_boundary_at_most) Body block: >>" \
		<< body_block << "<<" << std::endl;

	if (!start)
		return "Incomplete recv.";

	return in_body.substr(start, end - start);
}

int ws_reply_instance::is_202(ws_server_instance& si)
{
	std::string file_name;
	std::string dir_name;
	std::string mp_block;

	if (si.in_header.method == "POST")
	{
		si.max_size = si.config.get<int>("client_max_body_size");
		DataFold loc = get_location_config(si);
		while (loc.loop())
		{
			if (loc.key == "client_max_body_size")
			{
				si.max_size = loc.get<int>("client_max_body_size");
				break ;
			}
		}

		if (si.in_header.content_type.find("multipart") == 0)
		{
			si.multipart_type = word_from(si.in_header.content_type,
					si.in_header.content_type.find("/") + 1);
			si.boundary = si.in_header.content_type.substr( \
					(si.in_header.content_type.find("boundary=") + 9));
		}

		verbose(1) << si << std::endl;
		mp_block = si.read_fd_for_boundary_at_most();

		file_name = "file_name";
		dir_name = "dir_name";

		verbose(1) << "(webserv) " << file_name << \
			" will be saved into " << dir_name << \
			"." << std::endl;

		set_code(202, "Accepted (Mocked)");
		out_body = "anything";
		return 202;
	}
	return 0;
}
