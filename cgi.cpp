/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/26 17:26:51 by fde-capu          #+#    #+#             */
/*   Updated: 2022/09/28 23:14:38 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

void ws_reply_instance::cgi_setenv(ws_server_instance& si, std::string path_info)
{
	setenv("REQUEST_METHOD", si.in_header.method.c_str(), 1);
	setenv("SERVER_PROTOCOL", si.in_header.protocol.c_str(), 1);
	setenv("PATH_INFO", path_info.c_str(), 1);
}

void ws_reply_instance::cgi_write_into_child(ws_server_instance& si, int u_pipe)
{
	int V(1);
	size_t written_bytes(0);
	size_t write_into_child;
	size_t w;

	write_into_child = si.is_chunked() ? si.chunked_content.length() :
		si.is_multipart() ? si.multipart_content.length() :
		si.in_body.length();
	while (write_into_child)
	{
		if (si.is_chunked())
			w = write(u_pipe, static_cast<const void*>(si.chunked_content.c_str()), write_into_child);
		else if (si.is_multipart())
			w = write(u_pipe, static_cast<const void*>(si.multipart_content.c_str()), write_into_child);
		else
			w = write(u_pipe, static_cast<const void*>(si.in_body.c_str()), write_into_child);
		write_into_child -= w < write_into_child ? w : write_into_child;
		written_bytes += w;
		verbose(V) << "(cgi_write_into_child) written_bytes = " << written_bytes << std::endl;
	}
}

int ws_reply_instance::cgi_pipe(ws_server_instance& si, const std::vector<std::string>& argv)
{
	int V(1);
	int pipe_pc[2] = {0, 0};
	int pipe_cp[2] = {0, 0};
	pid_t child_pid = -1;

	if (pipe(pipe_pc) == -1)
		throw std::domain_error("(cgi_pipe) Cannot pipe for cgi (parent->child).");
	if (pipe(pipe_cp) == -1)
		throw std::domain_error("(cgi_pipe) Cannot pipe for cgi (child->parent).");
	child_pid = fork();
	if (child_pid < 0)
		throw std::domain_error("(cgi_pipe) Forking went wrong.");
	if (child_pid == 0) // Child
	{
		cgi_setenv(si, argv[0]);
		dup2(pipe_cp[1], STDOUT_FILENO);
		dup2(pipe_pc[0], STDIN_FILENO);
		close(pipe_pc[0]);
		close(pipe_pc[1]);
		close(pipe_cp[0]);
		close(pipe_cp[1]);
		std::vector<char*> vec_cp = StringTools::vecstr2veccharp(argv);
		execv(vec_cp[0], vec_cp.data());
		throw std::domain_error("(webserv) This line should never be reached.");
	}
	else // Parent
	{
		close(pipe_pc[0]);
		close(pipe_cp[1]);
		if (si.in_header.is_post())
			cgi_write_into_child(si, pipe_pc[1]);
		close(pipe_pc[1]);
		out_body = CircularBuffer(pipe_cp[0]);
		close(pipe_cp[0]);
		verbose(V) << "(Parent) Got >>>" << LONG(out_body) << "<<<" << std::endl;
		header_from_body();
		if (si.in_header.is_post())
		{
			set_code(202, "Accepted");
			return 202;
		}
		else
		{
			set_code(200, "OK");
			return 200;
		}
	}
	return 0;
}

void ws_reply_instance::header_from_body()
{
	size_t dbreak[3];
	size_t h_body(std::string::npos);
	size_t dblen;

	out_header.status = atoi(StringTools::query_for("Status", out_body).c_str());
	out_header.status_msg = StringTools::query_for(itoa(out_header.status), out_body);
	out_header.content_type = StringTools::query_for("Content-Type", out_body);
	out_header.charset = StringTools::query_for("charset", out_body);
	dbreak[0] = out_body.find("\r\n\r\n");
	dbreak[1] = out_body.find("\n\n");
	dbreak[2] = out_body.find("\r\r");
	for (size_t i = 0; i < 3; i++)
		h_body = h_body > dbreak[i] ? dbreak[i] : h_body;
	dblen = h_body == dbreak[0] ? 4 : 2;
	if (h_body != std::string::npos)
		out_body = out_body.substr(h_body + dblen);
}

int ws_reply_instance::cgi_prepare(ws_server_instance& si, std::string program)
{
	int V(1);

	program += " " + si.location_path();
	verbose(V) << "(cgi_prepare) program " << program << std::endl;
	std::vector<std::string> arg_vec(StringTools::split(program, " "));
	verbose(V) << "(cgi_prepare) arg_vec " << arg_vec.data() << std::endl;
	std::string file_test(arg_vec[0]);
	verbose(V) << "(cgi_prepare) file_test " << file_test << std::endl;
	std::string root_path(si.root_config.getValStr("root") + "/" \
			+ si.config.getValStr("root") + "/");
	verbose(V) << "(cgi_prepare) root_path " << root_path << std::endl;
	std::string full_program;
	if (FileString::exists(root_path + "/" + file_test))
		full_program = root_path + "/" + file_test;
	else if (FileString::exists(file_test))
		full_program = file_test;
	else
		return bad_gateway(si.custom_error(502));
	stool.remove_dup_char(full_program, '/');
	verbose(V) << "(cgi_prepare) full_program " << full_program << std::endl;
	std::vector<std::string> argv(arg_vec.size());
	argv[0] = full_program;
	for (size_t i = 1; i < arg_vec.size(); i++)
	{
		verbose(V) << "(cgi_prepare) arg_vec[i] in: " << arg_vec[i] << std::endl;
		if (FileString::exists(root_path + arg_vec[i]))
		{
			argv[i] = root_path + arg_vec[i];
			stool.remove_dup_char(argv[i], '/');
		}
		else
		{
			argv[i] = arg_vec[i];
		}
		verbose(V) << "(cgi_prepare) argv[i] out: " << argv[i] << std::endl;
	}
	std::string syscall;
	for (size_t i = 0; i < argv.size(); i++)
		syscall += argv[i] + " ";
	verbose(1) << "(webserv:cgi) " << syscall << std::endl;
	return cgi_pipe(si, argv);
}

int ws_reply_instance::is_cgi_exec(ws_server_instance& si)
{
	int V(1);

	si.cgi_flag = false;
	verbose(V) << "(is_cgi_exec) si.location " << si.location_path() << std::endl;
	if (!FileString::exists(si.location_path()))
	{
		set_code(421, "Missdirected Request");
		out_body = TemplateError::page(421, si.custom_error(421));
		return 421;
	}
	DataFold cgi_vec(si.config.get<DataFold>("cgi"));
	verbose(V) << "(is_cgi_exec) cgi_vec " << cgi_vec << std::endl;
	std::string call_extension = StringTools::get_file_extension(si.in_header.directory);
	verbose(V) << "(is_cgi_exec) call_extension " << call_extension << std::endl;
	if (call_extension == "" || cgi_vec.empty())
		return 0;
	while (cgi_vec.loop())
	{
		std::vector<std::string> cgi_params(StringTools::split(cgi_vec.val, " "));
		std::string cgi_extension = cgi_params[0];
		verbose(V) << "(is_cgi_exec) cgi_extension " << cgi_extension << std::endl;
		if (cgi_extension != call_extension)
			continue ;
		std::string cgi_children = cgi_vec.val.substr(cgi_extension.length() + 1);
		verbose(V) << "(is_cgi_exec) cgi_children " << cgi_children << std::endl;
		si.cgi_flag = true;
		return cgi_prepare(si, cgi_children);
	}
	return 0;
}

bool ws_server_instance::is_cgi() const
{ return cgi_flag; }
