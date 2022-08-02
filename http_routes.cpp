/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_routes.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/29 15:31:47 by fde-capu          #+#    #+#             */
/*   Updated: 2022/08/02 16:39:18 by fde-capu         ###   ########.fr       */
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
			out_body = "BODY FOR 301";
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
		out_body = "BODY FOR 403";
		return 403;
	}
	return 0;
}

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
	if (si.in_header.method == "PUT")
	{
		set_code(501, "Not Implemented");
		out_body = "BODY FOR 501";
		return 501;
	}
	return 0;
}

int ws_reply_instance::is_405(ws_server_instance& si)
{
	bool method_accepted(false);

	DataFold accepted_methods = si.location_get(
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
	out_body = "BODY FOR 405";
	return 405;
}

int ws_reply_instance::is_cgi(ws_server_instance& si)
{
	verbose(1) << "(is_cgi) " << si << std::endl;
	return 0;
}

int ws_reply_instance::is_404(ws_server_instance& si)
{
	DataFold indexes;

	if (si.in_header.method != "GET")
		return 0;
	indexes = si.location_get("index");
	while (indexes.loop())
	{
		file_name = si.location_path(indexes.val);
		verbose(1) << "(is_404) Fetching " << file_name \
			<< std::endl;
		if (FileString::exists(file_name))
			return 0;
	}
	set_code(404, "File Not Found");
	out_body = "BODY FOR 404";
	return 404;
}

int ws_reply_instance::is_413_507(ws_server_instance& si)
{
	static int V(1);
	bool chunked(stool.is_equal_insensitive(si.in_header.transfer_encoding, "chunked"));

	verbose(V) << "(is_413_507) max_size: " << si.max_size << "." << std::endl;

	if (!si.is_multipart() && \
		static_cast<size_t>(si.in_header.content_length) > si.max_size)
	{
		set_code(413, "Payload Too Large (Declared Size)");
		out_body = "BODY FOR 413";
		return 413;
	}

	verbose(V) << "(is_413_507) content_type: " << si.in_header.content_type << std::endl;
	verbose(V) << "(is_413_507) transfer_encoding: " << si.in_header.transfer_encoding << std::endl;

	if (si.is_multipart() || chunked)
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

	verbose(V) << "(is_413_507) Multipart content accounts for " \
		<< si.multipart_content.length() << " bytes." \
		<< std::endl;
	verbose(V) << "(is_413_507) Non-multipart accounts for " \
		<< si.in_body.length() << " bytes." << std::endl;
	verbose(5) << "(is_413_507) in_body >>" << si.in_body << "<<" << std::endl;
	verbose(5) << "(is_413_507) multipart_content >>" << \
		si.multipart_content << "<<" << std::endl;

	if ((!si.is_multipart() && si.in_body.length() > si.max_size)
	|| (si.is_multipart() && si.multipart_content.length() > si.max_size))
	{
		set_code(413, "Payload Too Large (Deceptive Declaration or Excess)");
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

int ws_reply_instance::is_200(ws_server_instance& si)
{
	if (si.in_header.method != "GET")
		return 0;

	FileString from_file(file_name.c_str());
	out_body = from_file.content();
	if (from_file.exists())
	{
		verbose(2) << "(is_200) out_body >" << out_body << "<" \
			<< std::endl;
		set_code(200, "OK");
		return 200;
	}
	return 0;
}

int ws_reply_instance::is_201(ws_server_instance& si)
{
	static size_t O_LIM(15);
	static int V(1);

	std::string full_path;
	std::string mp_block;
	std::string data_simple;
	std::string* data;

	if (si.in_header.method == "POST")
	{
		full_path = si.location_path(si.multipart_filename);
		if (si.is_multipart())
			data = &si.multipart_content;
		else
			data = &si.in_body;

		data_simple = data->length() <= O_LIM ? *data : \
			data->substr(0, O_LIM) + "... (len " + itoa(data->length()) + ")";
		verbose(V) << "(webserv) >" << data_simple << \
			"< will be saved into " << full_path << \
			"." << std::endl;

		FileString::write(full_path, *data);

		set_code(201, "Accepted");
		out_body = "BODY FOR 201";
		return 201;
	}
	return 0;
}
