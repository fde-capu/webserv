/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_routes.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/29 15:31:47 by fde-capu          #+#    #+#             */
/*   Updated: 2022/10/05 18:56:53 by fde-capu         ###   ########.fr       */
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
		out_body = TemplateError::page(501, si.custom_error(501));
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
			out_body = TemplateError::page(301);
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
	out_body = TemplateError::page(405, si.custom_error(405));
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
		out_body = TemplateError::page(403, si.custom_error(403));
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
				out_body = TemplateError::page(403, si.custom_error(403));
				return 403;
			}
		}
		verbose(V) << "(is_404) autoindex: " << autoindex << std::endl;
		set_code(404, "File Not Found");
		out_body = TemplateError::page(404, si.custom_error(404));
		return 404;
	}
	if (si.in_header.method == "DELETE")
	{
		if (FileString::exists(request))
			return 0;
		set_code(404, "File Not Found");
		out_body = TemplateError::page(404, si.custom_error(404));
		return 404;
	}
	return 0;
}

int ws_reply_instance::is_413_507_422(ws_server_instance& si)
{ return read_limits(si); }

int ws_reply_instance::is_424(ws_server_instance& si)
{
	if (is_equal_insensitive(si.in_header.expect, "100-continue"))
	{
		verbose(2) << "(is_424) Because of " << si.in_header.expect << \
			", and sizes are ok, would return 100-continue. " \
			"webserv must always close connection, so, instead, " \
			"will return 424 Failed Dependency." << std::endl;

		set_code(424, "Failed Dependency");
		out_body = TemplateError::page(424, si.custom_error(424));
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
		out_body = TemplateError::page(400);
		return 400;
	}
	return 0;
}

int ws_reply_instance::is_201(ws_server_instance& si)
{
	static int V(1);

	std::string full_path;
	std::string mp_block;
	std::string* data;

	if (si.in_header.is_post())
	{
		full_path = si.location_path(si.multipart_filename);
		if (si.is_multipart())
			data = &si.multipart_content;
		else if (si.is_chunked())
			data = &si.chunked_content;
		else
			data = &si.in_body;

		verbose(V) << "(webserv) >" << SHORT((*data)) << \
			"< will be saved into " << full_path << \
			"." << std::endl;

		FileString::write(full_path, *data);
		set_code(201, "Created");
		out_body = TemplateError::page(201);
		return 201;
	}
	return 0;
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
		set_code(200, "OK");
		out_body = FileString(file_name.c_str()).content();
		verbose(V) << "(is_200) out_body" << std::endl << SHORT(out_body) << std::endl;
		return 200;
	}
	if (si.in_header.method == "DELETE")
	{
		verbose(V) << "(is_200) will delete " << request << std::endl;
		if (std::remove(request.c_str()) != 0)
		{
			set_code(422, "Unprocessable Entity");
			out_body = TemplateError::page(422, si.custom_error(422));
			return 422;
		}
		else
		{
			set_code(200, "OK");
			out_body = TemplateError::page(200);
			return 200;
		}
	}
	return 0;
}
