/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_routes.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/29 15:31:47 by fde-capu          #+#    #+#             */
/*   Updated: 2022/07/25 14:01:03 by fde-capu         ###   ########.fr       */
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
	if (method_accepted)
		return 0;
	set_code(405, "Method Not Allowed");
	return 405;
}

int ws_reply_instance::is_404(ws_server_instance& si)
{
	DataFold indexes;
	std::string file_name;

	if (si.in_header.method == "GET")
	{
		indexes = si.config.get("index");
		while (indexes.loop())
		{
			file_name = si.root_config.getValStr("root") \
						+ "/" + si.val("root") \
						+ si.in_header.directory \
						+ "/" + indexes.val;
			stool.remove_rep_char(file_name, '/');
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

	if (si.in_header.method == "GET")
	{
		indexes = si.config.get("index");
		while (indexes.loop())
		{
			file_name = si.root_config.getValStr("root") \
						+ "/" + si.val("root") \
						+ si.in_header.directory \
						+ "/" + indexes.val;
			stool.remove_rep_char(file_name, '/');
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
	if (is_multipart())
	{
		multipart_type = word_from(in_header.content_type,
				in_header.content_type.find("/") + 1);
		boundary = in_header.content_type.substr( \
				(in_header.content_type.find("boundary=") + 9));
		payload_start = in_body.find(boundary);
		if (payload_start == std::string::npos)
			payload_start = 0;
		else
			payload_start = in_body.find("\r\n", payload_start \
				+ boundary.length()) + 2;
		payload_end = in_body.find(boundary, payload_start);
		payload_end = payload_end == std::string::npos ? \
					  in_body.length() : payload_end - 4;
		body_start = in_body.find("\r\n\r\n", payload_start);
		body_start = body_start == std::string::npos ? 0 : body_start;
		body_start += body_start ? 4 : 0;
		body_end = payload_end;

		multipart_content_disposition = StringTools::query_for( \
			"Content-Disposition", in_body);
		multipart_name = StringTools::query_for("name", in_body);
		multipart_filename = StringTools::query_for("filename", in_body);
		multipart_content_type = StringTools::query_for("Content-Type", in_body);
	}
	else
	{
		payload_start = 0;
		payload_end = in_body.length();
		body_start = payload_start;
		body_end = payload_end;
	}
}

bool ws_server_instance::is_multipart() const
{ return in_header.content_type.find("multipart") == 0; }

int ws_reply_instance::is_413(ws_server_instance& si)
{
	verbose(3) << "(is_413) max_size: " << si.max_size << "." << std::endl;

	if (!si.is_multipart() && static_cast<size_t>(si.in_header.content_length) > si.max_size)
	{
		set_code(413, "Payload Too Large (Declared Size)");
		out_body = "BODY FOR 413";
		return 413;
	}

	if (si.is_multipart())
	{
		try
		{
			si.read_more();
		}
		catch (std::exception& e)
		{
			set_code(507, "Insufficient Storage");
			out_body = "BODY FOR 507";
			return 507;
		}
	}

	verbose(4) << "(is_413) Multipart content accounts for " \
		<< si.multipart_content.length() << " bytes." \
		<< std::endl;
	verbose(4) << "(is_413) Non-multipart accounts for " \
		<< si.in_body.length() << " bytes." << std::endl;
	verbose(5) << "(is_413) in_body >>" << si.in_body << "<<" << std::endl;
	verbose(5) << "(is_413) multipart_content >>" << si.multipart_content << \
		"<<" << std::endl;

	if ((!si.is_multipart() && si.in_body.length() > si.max_size)
	|| (si.is_multipart() && si.multipart_content.length() > si.max_size))
	{
		set_code(413, "Payload Too Large (Deceptive Declaration)");
		out_body = "BODY FOR 413";
		return 413;
	}
	else
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
		out_body = "BODY FOR 424";
		return 424;
	}
	return 0;
}

int ws_reply_instance::is_529(ws_server_instance& si)
{
	verbose(1) << "(is_529) Multipart: " << si.is_multipart() << \
		", multipart-content-length: " << \
		si.multipart_content.length() << ", in_body-length: " <<  \
		si.in_body.length() << ", expected_full_load: " << si.expected_full_load << \
		"." << std::endl;

	if (si.is_multipart() && si.in_body.length() < si.expected_full_load)
	{
		set_code(529, "Site is overloaded");
		out_body = "BODY FOR 529";
		return 529;
	}
	return 0;
}

int ws_reply_instance::is_202(ws_server_instance& si)
{
	std::string dir_name;
	std::string mp_block;
	DataFold loc = si.get_location_config();
	std::string loc_choice;
	std::string data_simple;

	if (si.in_header.method == "POST")
	{
		loc_choice = loc.getValStr("root");
		if (loc_choice.empty() || loc_choice.at(0) != '/')
			loc_choice = si.config.getValStr("root") + "/" + loc_choice;
		dir_name = si.root_config.getValStr("root") + \
			"/" + loc_choice + "/" + si.multipart_filename;
		stool.remove_rep_char(dir_name, '/');

		std::string data(si.multipart_content);
		data_simple = data.length() <= 202 ? data : \
			"(large file, will not print)";
		verbose(5) << "(webserv) >" << data_simple << \
			"< will be saved into " << dir_name << \
			"." << std::endl;

		FileString::write(dir_name, si.multipart_content);

		set_code(202, "Accepted");
		out_body = "BODY FOR 202";
		return 202;
	}
	return 0;
}
