/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_routes.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/29 15:31:47 by fde-capu          #+#    #+#             */
/*   Updated: 2022/06/30 16:25:59 by fde-capu         ###   ########.fr       */
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
	DataFold locations(si.config.get<DataFold>("location"));

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
	DataFold locations(si.config.get<DataFold>("location"));

	if (si.in_header.method == "GET")
	{
		indexes = si.config.get("index");
		while (indexes.loop())
		{
			file_name = si.root_config.getValStr("root") \
						+ "/" + si.val("root") \
						+ si.in_header.directory \
						+ "/" + indexes.val;
			//		stool.remove_rep_char(file_name, '/'); // The kernel doesn't really care.
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

int ws_reply_instance::is_202(ws_server_instance& si)
{
	std::string file_name;
	std::string dir_name;
	int max_size;

	if (si.in_header.method == "POST")
	{
		DataFold locations(si.config.get<DataFold>("location"));
		DataFold loc;

		max_size = si.config.get<int>("client_max_body_size");
		if (!locations.empty())
		{
			while (locations.loop())
			{
				loc = locations.val;
				if (loc.getValStr("uri") == si.in_header.directory)
				while (loc.loop())
					if (loc.key == "client_max_body_size")
						max_size = loc.get<int>("client_max_body_size");
			}
		}

		verbose(1) << "(webserv) " << si.in_header.directory << " accepting at most " << max_size << " bytes." << std::endl;

		file_name = "file_name";
		dir_name = "dir_name";

		verbose(1) << "(webserv) " << file_name << \
			" will be saved into " << dir_name << \
			"." << std::endl;

		set_code(202, "Accepted");
		out_body = "anything";
		return 202;
	}
	return 0;
}
