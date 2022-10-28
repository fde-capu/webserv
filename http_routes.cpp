/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_routes.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/29 15:31:47 by fde-capu          #+#    #+#             */
/*   Updated: 2022/10/28 23:04:33 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

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
		template_page(501, custom_error(501, si));
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
			template_page(301);
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
	template_page(405, custom_error(405, si));
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
		template_page(403, custom_error(403, si));
		return 403;
	}
	return 0;
}

int ws_reply_instance::is_404(ws_server_instance& si)
{
	int V(4);
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
				template_page(403, custom_error(403, si));
				return 403;
			}
		}
		verbose(V) << "(is_404) autoindex: " << autoindex << std::endl;
		set_code(404, "File Not Found");
		template_page(404, custom_error(404, si));
		return 404;
	}
	if (si.in_header.method == "DELETE")
	{
		if (FileString::exists(request))
			return 0;
		set_code(404, "File Not Found");
		template_page(404, custom_error(404, si));
		return 404;
	}
	return 0;
}

int ws_reply_instance::is_413_507_422(ws_server_instance& si)
{
	int V(4);
	int pos_status(0);

	verbose(V) << "(is_413_507_422) max_size: " << si.max_size << "." \
		<< std::endl;
	verbose(V) << "(is_413_507_422) content_type: " << \
		si.in_header.content_type << std::endl;

	pos_status = si.process_post();
	if (si.exceeded_limit)
	{
		set_code(413, "Payload Too Large");
		template_page(413, custom_error(413, si));
		return 413;
	}
	if (si.in_header.content_length > MEMORY_LIMIT)
	{
		verbose(V - 2) << "(is_413_507_422) Client declared " << si.in_header.content_length << "." << std::endl;
		set_code(507, "Insufficient Resources");
		template_page(507, custom_error(507, si));
		return 507;
	}
	if (pos_status == 422)
	{
		set_code(422, "Unprocessable Entity");
		template_page(422, custom_error(422, si));
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
		verbose(3) << "(is_424) Because of " << si.in_header.expect << \
			", and sizes are ok, would return 100-continue. " \
			"webserv must always close connection, so, instead, " \
			"will return 424 Failed Dependency." << std::endl;

		set_code(424, "Failed Dependency");
		template_page(424, custom_error(424, si));
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
		template_page(400);
		return 400;
	}
	return 0;
}

int ws_reply_instance::is_201(ws_server_instance& si)
{
	int V(4);

	if (si.in_header.is_post() && !save_canceled())
	{
		full_path = si.location_path(si.multipart_filename);
		verbose(V) << "(is_201) Opening " << full_path << "." << std::endl;
		file_save = open(full_path.c_str(), O_WRONLY | O_CREAT | O_NONBLOCK | O_CLOEXEC, S_IRUSR | S_IWUSR);
		if (file_save == -1)
		{
			file_save = 0;
			set_code(429, "Too Many Requests");
			template_page(429);
			return 429;
		}
		if (fcntl(file_save, F_SETFL, O_NONBLOCK) == -1)
			throw std::domain_error(itoa(si.fd) + " (is_201) Could not set non-blocking file.");
		verbose(V) << "(is_201) as " << file_save << "." << std::endl;
		poll_list.push_back(WebServ::make_in_out_fd(file_save));
		set_code(201, "Created");
		template_page(201);
		return 201;
	}
	return 0;
}

int ws_reply_instance::is_200(ws_server_instance& si)
{
	int V(3);
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
		file_page = open(file_name.c_str(), O_CLOEXEC | O_NONBLOCK | O_RDONLY);
		if (file_page == -1)
			throw std::domain_error(itoa(si.fd) + " (is_200) Cannot open file to load page.");
		if (fcntl(file_page, F_SETFL, O_NONBLOCK) == -1)
			throw std::domain_error(itoa(si.fd) + " (is_200) Could not set non-blocking file.");
		poll_list.push_back(WebServ::make_in_out_fd(file_page));
		to_work_load = true;
		set_code(200, "OK");
		return 200;
	}
	if (si.in_header.method == "DELETE")
	{
		verbose(V) << "(is_200) will delete " << request << std::endl;
		if (std::remove(request.c_str()) != 0)
		{
			set_code(422, "Unprocessable Entity");
			template_page(422, custom_error(422, si));
			return 422;
		}
		else
		{
			set_code(200, "OK");
			template_page(200);
			return 200;
		}
	}
	return 0;
}
