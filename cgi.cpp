/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/26 17:26:51 by fde-capu          #+#    #+#             */
/*   Updated: 2022/10/21 20:17:00 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

bool ws_reply_instance::cgi_dumping(ws_server_instance& si)
{
	int V(1);
	size_t ASYNC_CHUNK_SIZE(1000000);
	std::string* data;
	int poll_count;
	int TIME_OUT = 0; // non-blocking.
	int sbytes;
	size_t wr_size;

	if (!si.in_header.is_post() || save_canceled())
		return false;
	if (si.is_chunked() && !si.chunk_finished)
	{
		si.mount_chunked();
		verbose(V) << "(cgi_dumping) Mounted chunked data." << std::endl;
		return true;
	}
	if (si.is_multipart() && !si.multipart_finished)
	{
		si.mount_multipart();
		verbose(V) << "(cgi_dumping) Mounted multipart data." << std::endl;
		return true;
	}
	if (si.is_multipart())
		data = &si.multipart_content;
	else if (si.is_chunked())
		data = &si.chunked_content;
	else
		data = &si.in_body;
	
	poll_count = poll(&poll_list[0], poll_list.size(), TIME_OUT);
	if (poll_count == -1)
		throw std::domain_error("(webserv) Poll error.");

	for (size_t i = 0; i < poll_list.size(); i++)
	{
		if (!poll_list[i].revents)
			continue ;
		if (poll_list[i].revents & POLLOUT)
		{
			wr_size = data->length() > ASYNC_CHUNK_SIZE ? ASYNC_CHUNK_SIZE : data->length();
//			verbose(V) << "(cgi_dumping) Writing into " << poll_list[i].fd << std::endl;
			sbytes = write(poll_list[i].fd, data->c_str(), wr_size);
			if (sbytes < 0)
			{
				verbose(V) << " - NEGATIVE SBYTES" << std::endl;
				return false;
			}
			if (sbytes > 0)
				StringTools::consume_bytes(*data, sbytes);
			if (si.is_multipart())
				data = &si.multipart_content;
			else if (si.is_chunked())
				data = &si.chunked_content;
			else
				data = &si.in_body;
//			verbose(V) << " - Dumped " << sbytes << ", " << data->length() << " left." << std::endl;
			if (data->length() == 0)
			{
//				std::vector<struct pollfd>::iterator position(&poll_list[i]);
//				poll_list.erase(position);
//				verbose(V) << poll_list[i].fd << " - Finished dumping (removed)." << std::endl;
				verbose(V) << " - Finished dumping (not removed)." << std::endl;
				return false;
			}
		}
	}
	return true;
}

bool ws_reply_instance::cgi_receiving()
{
	int V(2);
	size_t ASYNC_CHUNK_SIZE(10000000);
	int CGI_TIMEOUT(50);
	int poll_count;
	int TIME_OUT = 0; // non-blocking.
	int rbytes;
	char* buffer = static_cast<char*>(malloc(ASYNC_CHUNK_SIZE));

	if (first_time)
	{
		verbose(V) << "(cgi_receiving) First time." << std::endl;
		verbose(V) << "- Current body: " << SHORT(out_body) << std::endl;
		first_time = false;
		chronometer.btn_reset();
	}
	if (chronometer > CGI_TIMEOUT)
	{
		verbose(V) << chronometer << " ----- !!! ----- CGI timed out." << std::endl;
		return false;
	}
	poll_count = poll(&poll_list[0], poll_list.size(), TIME_OUT);
	if (poll_count == -1)
		throw std::domain_error("(webserv) Poll error.");
	for (size_t i = 0; i < poll_list.size(); i++)
	{
		if (!poll_list[i].revents)
			continue ;
		if (poll_list[i].revents & POLLIN)
		{
			rbytes = read(poll_list[i].fd, buffer, ASYNC_CHUNK_SIZE);
			verbose(V) << "(cgi_receiving) " << rbytes << " bytes from " << poll_list[i].fd << "." << std::endl;
			if (rbytes < 0)
				return false;
			if (rbytes)
			{
				verbose(V) << "(cgi_receiving) Append, reset." << std::endl;
				out_body.append(buffer, rbytes);
				chronometer.btn_reset();
				return true;
			}
			else
			{
				verbose(V) << "(cgi_receiving) Nothing." << std::endl;
				return false;
			}
		}
	}
	return true;
}

int ws_reply_instance::cgi_pipe(ws_server_instance& si, const std::vector<std::string>& argv)
{
	int V(1);

	child_pid = -1;
	if (pipe2(pipe_pc, O_CLOEXEC | O_DIRECT | O_NONBLOCK) == -1)
		throw std::domain_error("(cgi_pipe) Cannot pipe for cgi (parent->child).");
	if (pipe2(pipe_cp, O_CLOEXEC | O_DIRECT | O_NONBLOCK) == -1)
		throw std::domain_error("(cgi_pipe) Cannot pipe for cgi (child->parent).");
	child_pid = fork();
	if (child_pid < 0)
		throw std::domain_error("(cgi_pipe) Fork went wrong.");
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
		throw std::domain_error("(cgi_pipe) This line should never be reached.");
	}
	else // Parent
	{
		close(pipe_pc[0]);
		close(pipe_cp[1]);
		if (si.in_header.is_post())
		{
			WebServ::set_non_blocking(pipe_pc[1]);
			poll_list.push_back(WebServ::make_in_out_fd(pipe_pc[1]));
			dumping_to_cgi = true;
			verbose(V) << "(cgi_pipe) Set dump-to-cgi " << pipe_pc[1] << std::endl;
		}
		else
		{
			close(pipe_pc[1]);
		}
		WebServ::set_non_blocking(pipe_cp[0]);
		poll_list.push_back(WebServ::make_in_out_fd(pipe_cp[0]));
		getting_from_cgi = true;
		verbose(V) << "(cgi_pipe) Set get-from-cgi " << pipe_cp[0] << std::endl;
	}
	return 1;
}

int ws_reply_instance::cgi_prepare(ws_server_instance& si, std::string program)
{
	int V(3);

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
	verbose(CRITICAL) << "(webserv:cgi) " << syscall << std::endl;
	return cgi_pipe(si, argv);
}

int ws_reply_instance::is_cgi_exec(ws_server_instance& si)
{
	int V(3);
	std::string call_extension;
	std::string cgi_params_str;
	std::vector<std::string> cgi_params;
	std::string cgi_extension;
	std::string cgi_children;

	si.cgi_flag = false;
	verbose(V) << "(is_cgi_exec) si.location " << si.location_path() << std::endl;
	call_extension = StringTools::get_file_extension(si.in_header.directory);
	verbose(V) << "(is_cgi_exec) call_extension " << call_extension << std::endl;
	if (call_extension == "")
		return 0;
	if (!si.is_chunked() && !FileString::exists(si.location_path()) && !si.in_header.is_post())
	{
		set_code(421, "Missdirected Request");
		out_body = TemplatePage::page(421, si.custom_error(421));
		return 421;
	}
	DataFold cgi_vec(si.config.get<DataFold>("cgi"));
	verbose(V) << "(is_cgi_exec) cgi_vec " << cgi_vec << std::endl;
	if (cgi_vec.empty())
		return 0;
	if (cgi_vec.size() == 1)
	{
		verbose(V) << "(is_cgi_exec) size == 1" << std::endl;
		cgi_params_str = cgi_vec.getValStr("cgi");
		verbose(V) << "(is_cgi_exec) cgi_params_str " << cgi_params_str << std::endl;
		cgi_params = StringTools::split(cgi_params_str, " ");
		cgi_extension = cgi_params[0];
		verbose(V) << "(is_cgi_exec) cgi_extension " << cgi_extension << std::endl;
		cgi_children = cgi_params_str.substr(cgi_extension.length() + 1);
		verbose(V) << "(is_cgi_exec) cgi_children " << cgi_children << std::endl;
		si.cgi_flag = true;
		return cgi_prepare(si, cgi_children);
	}
	while (cgi_vec.loop())
	{
		verbose(V) << "(is_cgi_exec) cgi_vec.val " << cgi_vec.val << std::endl;
		cgi_params = StringTools::split(cgi_vec.val, " ");
		cgi_extension = cgi_params[0];
		verbose(V) << "(is_cgi_exec) cgi_extension " << cgi_extension << std::endl;
		if (cgi_extension != call_extension)
			continue ;
		cgi_children = cgi_vec.val.substr(cgi_extension.length() + 1);
		verbose(V) << "(is_cgi_exec) cgi_children " << cgi_children << std::endl;
		si.cgi_flag = true;
		return cgi_prepare(si, cgi_children);
	}
	return 0;
}
