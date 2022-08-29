/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_routes.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/29 15:31:47 by fde-capu          #+#    #+#             */
/*   Updated: 2022/08/29 18:47:23 by fde-capu         ###   ########.fr       */
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

int ws_reply_instance::execute_cgi(ws_server_instance& si, std::string program)
{
	int V(1);
	verbose(V) << "(execute_cgi) si = " << si << std::endl;
	verbose(V) << "(execute_cgi) WILL HAPPEN HERE!!!" << std::endl;
	verbose(V) << "(execute_cgi) Program is: " << program << std::endl;
	return 1;
}

int ws_reply_instance::is_cgi(ws_server_instance& si)
{
	int V(1);
	std::vector<std::string> cgi(si.config.get_vector_str("cgi"));
	std::vector<std::string> cgi_accept(si.config.get_vector_str("cgi_accept"));
	std::string cgi_extension = cgi[0];
	std::string cgi_children = cgi[1];
	std::string call_method = si.in_header.method;
	bool good_method = false;
	for (size_t i = 0; i < cgi_accept.size(); i++)
	{
		verbose(V) << "(is_cgi) call: " << call_method << ", test " << cgi_accept[i] << std::endl;
		if (cgi_accept[i] == call_method)
			good_method = true;
	}
	if (!good_method)
		return 0;
	std::string call_extension = StringTools::get_file_extension(si.in_header.directory);
	if (call_extension == cgi_extension)
		return execute_cgi(si, cgi_children);
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
	static int VPRINTLIM(40);
	int pos_status(0);

	verbose(V) << "(is_413_507) max_size: " << si.max_size << "." \
		<< std::endl;
	verbose(V) << "(is_413_507) content_type: " << \
		si.in_header.content_type << std::endl;

	pos_status = si.read_more_general();
	si.set_sizes();
	if (si.exceeded_limit)
	{
		set_code(413, "Payload Too Large (Declared Size)");
		out_body = "BODY FOR 413";
		return 413;
	}
	if (pos_status == 507)
	{
		set_code(507, "Insufficient Resources");
		out_body = "BODY FOR 507";
		return 507;
	}
	if (pos_status == 422)
	{
		set_code(422, "Unprocessable Entity");
		out_body = "BODY FOR 422";
		return 422;
	}

	verbose(V) << "(is_413_507) in_body >>" << \
		si.in_body.substr(0, VPRINTLIM) << "<< len: " << si.in_body.length() << \
		std::endl;
	verbose(V) << "(is_413_507) multipart_content >>" << \
		si.multipart_content.substr(0, VPRINTLIM) << "<< len: " << \
		si.multipart_content.length() << std::endl;
	verbose(V) << "(is_413_507) chunked_content >>" << \
		si.chunked_content.substr(0, VPRINTLIM) << "<< len: " << \
		si.chunked_content.length() << std::endl;

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
