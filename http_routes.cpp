/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_routes.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/29 15:31:47 by fde-capu          #+#    #+#             */
/*   Updated: 2022/07/07 14:00:33 by fde-capu         ###   ########.fr       */
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

DataFold ws_server_instance::get_location_config()
{
	DataFold locations(config.get<DataFold>("location"));
	if (locations.empty()) return locations;
	while (locations.loop())
		if (DataFold(locations.val).getValStr("uri") == in_header.directory)
			return locations.val;
	return locations;
}

void ws_server_instance::boundary_start_end()
{
	payload_start = in_body.find(boundary);
	if (payload_start == std::string::npos)
		payload_start = 0;
	else
		payload_start = in_body.find("\r\n", payload_start + boundary.length()) + 2;
	payload_end = in_body.find(boundary, payload_start);
	payload_end = payload_end == std::string::npos ? \
		in_body.length() : payload_end - 4;
}

void ws_server_instance::body_start_end()
{
	body_start = in_body.find("\r\n\r\n", payload_start);
	body_start = body_start == std::string::npos ? 0 : body_start;
	body_start += body_start ? 4 : 0;
	body_end = payload_end;
}

void ws_server_instance::set_sizes()
{
	max_size = config.get<int>("client_max_body_size");
	DataFold loc = get_location_config();
	while (loc.loop())
	{
		if (loc.key == "client_max_body_size")
		{
			max_size = loc.get<int>("client_max_body_size");
			break ;
		}
	}
	if (is_multitype())
	{
		multipart_type = word_from(in_header.content_type,
				in_header.content_type.find("/") + 1);
		boundary = in_header.content_type.substr( \
				(in_header.content_type.find("boundary=") + 9));
		boundary_start_end();
		body_start_end();
	}
	else
	{
		payload_start = 0;
		payload_end = in_body.length();
		body_start = payload_start;
		body_end = payload_end;
	}
}

bool ws_server_instance::is_multitype() const
{ return in_header.content_type.find("multipart") == 0; }

std::string ws_server_instance::read_fd_for_boundary_at_most()
{
	std::string dir_name(in_header.directory);

	verbose(1) << "(read_fd_for_boundary_at_most) " << dir_name << \
		" accepting at most " << max_size << " bytes." << std::endl;

	std::string body_block;
	body_block = in_body.substr(body_start, body_end - body_start);

	verbose(1) << "(read_fd_for_boundary_at_most) Body block: >>" \
		<< body_block << "<<" << std::endl;

	if (payload_start == 0)
		verbose(1) << "(read_fd_for_boundary_at_most) Incomplete receivement." << std::endl;

	return in_body.substr(payload_start, payload_end - payload_start);
}

int ws_reply_instance::is_413(ws_server_instance& si)
{
	if (si.in_header.method != "POST")
	{
		if (static_cast<size_t>(si.in_header.content_length) \
				> si.max_size \
				|| si.in_body.length() > si.max_size)
		{
			set_code(413, "Payload Too Large");
			out_body = "BODY FOR 413";
			return 413;
		}
		return 0;
	}
	if (si.is_multitype())
	{
		verbose(1) << "(is_413) Will read " << \
			si.in_header.content_length << " bytes for " << \
			si.multipart_type << " using boundary >>" << si.boundary \
			<< "<<." << std::endl;
		verbose(1) << "(is_413) Payload start: " << si.payload_start << \
			", end: " << si.payload_end << "." << std::endl;
		verbose(1) << "(is_413) Body start: " << \
			si.body_start << ", end: " << si.body_end << "." << std::endl;

		size_t file_size(si.body_end - si.body_start);

		verbose(1) << "(is_413) This accounts for " \
			<< file_size << " bytes." << std::endl;

		if (file_size > si.max_size)
		{
			set_code(413, "Payload Too Large");
			out_body = "BODY FOR 413";
			return 413;
		}
	}
	return 0;
}

int ws_reply_instance::is_202(ws_server_instance& si)
{
	std::string file_name;
	std::string dir_name;
	std::string mp_block;

	if (si.in_header.method == "POST")
	{
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
