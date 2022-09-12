/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_routes.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/29 15:31:47 by fde-capu          #+#    #+#             */
/*   Updated: 2022/09/12 21:06:35 by fde-capu         ###   ########.fr       */
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
	if (
		(si.in_header.method == "PUT")
	)
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
	out_body = "BODY FOR 405";
	return 405;
}

int ws_reply_instance::execute_cgi(ws_server_instance& si, std::string program)
{
	int V(1);
	size_t wr(0);
	if (program.find(" ") == std::string::npos)
		program = si.root_config.getValStr("root") + "/" \
			+ si.config.getValStr("root") + "/" + program;

	verbose(V) << "(execute_cgi) Program is: " << program << std::endl;

	if (!FileString::exists(program.substr(0, program.find(" "))))
	{
		set_code(502, "Bad Gateway");
		out_body = "BODY FOR 502";
		return 502;
	}

	pid_t child_pid = -1;
	pid_t wait_pid = -1;
	int child_status = -1;
	char* args[2] = {0, 0};
	int pipe_pc[2] = {0, 0};
	int pipe_cp[2] = {0, 0};

	if (pipe(pipe_pc) == -1)
		throw std::domain_error("(execute_cgi) Cannot pipe for cgi (parent->child).");
	if (pipe(pipe_cp) == -1)
		throw std::domain_error("(execute_cgi) Cannot pipe for cgi (child->parent).");

	child_pid = fork();
	if (child_pid < 0)
		throw std::domain_error("(execute_cgi) Forking went wrong.");
	if (child_pid == 0) // Child.
	{
		dup2(pipe_pc[0], STDIN_FILENO);
		dup2(pipe_cp[1], STDOUT_FILENO);
		close(pipe_pc[0]);
		close(pipe_pc[1]);
		close(pipe_cp[0]);
		close(pipe_cp[1]);
		args[0] = const_cast<char *>(program.c_str());
		setenv("REQUEST_METHOD", si.in_header.method.c_str(), 1);
		setenv("SERVER_PROTOCOL", si.in_header.protocol.c_str(), 1);
		setenv("PATH_INFO", program.c_str(), 1);
		execv(args[0], args);
		exit(502);
	}
	else // Parent.
	{
		if (si.is_chunked())
		{
			wr = write(pipe_pc[1], static_cast<const void*>(si.chunked_content.c_str()),\
				si.chunked_content.length());
		}
		else if (si.is_multipart())
		{
			wr = write(pipe_pc[1], static_cast<const void*>(si.multipart_content.c_str()),\
				si.multipart_content.length());
		}
		else
		{
			wr = write(pipe_pc[1], static_cast<const void*>(si.in_body.c_str()),\
				si.in_body.length());
		}
		verbose(V) << "(execute_cgi) wr = " << wr << std::endl;
		close(pipe_pc[0]);
		close(pipe_cp[1]);
		close(pipe_pc[1]);

		wait_pid = wait(&child_status);
		if (wait_pid < 0)
			throw std::domain_error("(execute_cgi) Coudn't wait.");

		out_body = CircularBuffer(pipe_cp[0]);
		close(pipe_cp[0]);
		verbose(V) << "(execute_cgi) Got >>>" << LONG(out_body) << "<<<" << std::endl;

		out_header.status = atoi(StringTools::query_for("Status", out_body).c_str());
		out_header.status_msg = StringTools::query_for(itoa(out_header.status), out_body);
		out_header.content_type = StringTools::query_for("Content-Type", out_body);
		out_header.charset = StringTools::query_for("charset", out_body);

		size_t h_body = out_body.find("\r\n\r\n");
		if (h_body != std::string::npos)
			out_body = out_body.substr(h_body + 4);

		verbose(V) << "(execute_cgi) out_body " << SHORT(out_body) << std::endl;
//		verbose(V) << "(execute_cgi) WIFEXITED(child_status) " << WIFEXITED(child_status) << std::endl;
		set_code(202, "Accepted");
		return 202;
	}
}

bool ws_server_instance::is_cgi() const
{ return cgi_flag; }

int ws_reply_instance::is_cgi_exec(ws_server_instance& si)
{
	std::string cgi_str(si.config.getValStr("cgi"));
	std::vector<std::string> cgi(si.config.get_vector_str("cgi"));
	if (cgi.empty())
		return 0;
	std::vector<std::string> cgi_accept(si.config.get_vector_str("cgi_accept"));
	std::string cgi_extension = cgi[0];
	std::string cgi_children = cgi_str.substr(cgi_str.find(cgi[0]) + cgi[0].length() + 1);
	std::string call_method = si.in_header.method;
	bool good_method = false;
	for (size_t i = 0; i < cgi_accept.size(); i++)
		if (cgi_accept[i] == call_method)
			good_method = true;
	if (!good_method)
		return 0;
	std::string call_extension = StringTools::get_file_extension(si.in_header.directory);
	if (call_extension == cgi_extension)
	{
		si.cgi_flag = true;
		return execute_cgi(si, cgi_children);
	}
	si.cgi_flag = false;
	return 0;
}

int ws_reply_instance::is_404(ws_server_instance& si)
{
	int V(1);
	std::string request;
	DataFold indexes;

	if (si.in_header.method != "GET")
		return 0;
	request = si.location_path();

	verbose(V) << "(is_404) request: " << request << std::endl;

	if (FileString::is_dir(request))
	{
		indexes = si.server_location_config("index");
		while (indexes.loop())
		{
			file_name = si.location_path(indexes.val);
			verbose(1) << "(is_404) Fetching " << file_name \
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
	set_code(404, "File Not Found");
	out_body = "BODY FOR 404";
	return 404;
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
	int V(1);
	std::string request;
	DataFold indexes;

	request = si.location_path();

	verbose(V) << "(is_200) request: " << request << std::endl;

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
	{
		file_name = request;
	}
	if (!FileString::exists(file_name))
		return 0;

	set_code(200, "OK");
	out_body = FileString(file_name.c_str()).content();

	verbose(V) << "(is_200) out_body" << std::endl << SHORT(out_body) << std::endl;

	return 200;
}

int ws_reply_instance::is_201(ws_server_instance& si)
{
	static int V(1);

	std::string full_path;
	std::string mp_block;
	std::string* data;

	if (si.in_header.method == "POST")
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
		set_code(201, "Accepted");
		out_body = "BODY FOR 201";
		return 201;
	}
	return 0;
}
