/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/26 17:26:51 by fde-capu          #+#    #+#             */
/*   Updated: 2022/09/26 21:51:46 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

int ws_reply_instance::cgi_pipe(ws_server_instance& si, std::vector<std::string> argv)
{
	int V(1);
	int pipe_pc[2] = {0, 0};
	int pipe_cp[2] = {0, 0};
	pid_t child_pid = -1;
//	size_t written_bytes(0);

	if (pipe(pipe_pc) == -1)
		throw std::domain_error("(execute_cgi) Cannot pipe for cgi (parent->child).");
	if (pipe(pipe_cp) == -1)
		throw std::domain_error("(execute_cgi) Cannot pipe for cgi (child->parent).");

	child_pid = fork();
	if (child_pid < 0)
		throw std::domain_error("(execute_cgi) Forking went wrong.");
	if (child_pid == 0) // Child
	{

argv.clear();
argv.push_back("/usr/bin/ls");
argv.push_back("/");
argv.push_back("-l");

		verbose(V) << "(Child)" << std::endl;
//		dup2(pipe_pc[0], STDIN_FILENO);
		dup2(pipe_cp[1], STDOUT_FILENO);
		close(pipe_pc[0]);
		close(pipe_pc[1]);
		close(pipe_cp[0]);
		close(pipe_cp[1]);
//		setenv("REQUEST_METHOD", si.in_header.method.c_str(), 1);
//		setenv("SERVER_PROTOCOL", si.in_header.protocol.c_str(), 1);
//		setenv("PATH_INFO", argv[0].c_str(), 1);
//		verbose(V) << "(execute_cgi) Child argv.data(): " << SHORT(std::string(*argv.data())) << std::endl;
//		execv(argv[0], argv.data());
char** argl = static_cast<char**>(calloc(argv.size() + 1, 1));
for (size_t i = 0; i < argv.size(); i++)
{
	argl[i] = static_cast<char*>(calloc(argv[i].length() + 1, 1));
	for (size_t j = 0; j < argv[i].length(); j++)
	{
		argl[i][j] = argv[i].at(j);
	}
}

size_t i = 0;
while (*argl[i])
{
//	std::cout << "))" << std::string(argl[i]) << "((" << std::endl;
	i++;
}

		execv(argl[0], argl);
		throw std::domain_error("This line should never be reached.");
	}
	else // Parent
	{
		pid_t wait_pid = -1;
		int child_status = -1;

		verbose(V) << "(Parent)" << std::endl;
//		size_t write_into_child;
//		size_t w;
//
//		write_into_child = si.is_chunked() ? si.chunked_content.length() :
//			si.is_multipart() ? si.multipart_content.length() :
//			si.in_body.length();
//		while (write_into_child)
//		{
//			if (si.is_chunked())
//				w = write(pipe_pc[1], static_cast<const void*>(si.chunked_content.c_str()), write_into_child);
//			else if (si.is_multipart())
//				w = write(pipe_pc[1], static_cast<const void*>(si.multipart_content.c_str()), write_into_child);
//			else
//				w = write(pipe_pc[1], static_cast<const void*>(si.in_body.c_str()), write_into_child);
//			write_into_child -= w < write_into_child ? w : write_into_child;
//			written_bytes += w;
//			verbose(V) << "(execute_cgi) written_bytes = " << written_bytes << std::endl;
//		}
		close(pipe_pc[0]);
		close(pipe_cp[1]);
		close(pipe_pc[1]);
		wait_pid = waitpid(child_pid, &child_status, 0);
		if (wait_pid < 0)
			throw std::domain_error("(execute_cgi) Coudn't wait.");
		verbose(V) << "(Parent) waited, wait_pid " << wait_pid << ", child_status " << child_status << std::endl;
		out_body = CircularBuffer(pipe_cp[0]);
		close(pipe_cp[0]);
		verbose(V) << "(Parent) Got >>>" << LONG(out_body) << "<<<" << std::endl;
//
//		out_header.status = atoi(StringTools::query_for("Status", out_body).c_str());
//		out_header.status_msg = StringTools::query_for(itoa(out_header.status), out_body);
//		out_header.content_type = StringTools::query_for("Content-Type", out_body);
//		out_header.charset = StringTools::query_for("charset", out_body);
//		size_t h_body = out_body.find("\r\n\r\n");
//		if (h_body != std::string::npos)
//			out_body = out_body.substr(h_body + 4);
//
//		verbose(V) << "(execute_cgi) out_body " << SHORT(out_body) << std::endl;
//		verbose(V) << "(execute_cgi) WIFEXITED (child_status) " << WIFEXITED(child_status) << std::endl;
//
//		set_code(202, "Accepted");
//		return 202;
		BREAK;
		(void)si;
	}
	return 0;
}

int ws_reply_instance::execute_cgi(ws_server_instance& si, std::string program)
{
	int V(1);

	verbose(V) << "(execute_cgi) program " << program << std::endl;
	std::vector<std::string> arg_vec(StringTools::split(program, " "));
	verbose(V) << "(execute_cgi) arg_vec " << arg_vec.data() << std::endl;
	std::string file_test(arg_vec[0]);
	verbose(V) << "(execute_cgi) file_test " << file_test << std::endl;
	std::string root_path(si.root_config.getValStr("root") + "/" \
			+ si.config.getValStr("root") + "/");
	verbose(V) << "(execute_cgi) root_path " << root_path << std::endl;
	std::string full_program;
	if (FileString::exists(root_path + file_test))
		full_program = root_path + file_test;
	else if (FileString::exists(file_test))
		full_program = file_test;
	else
		return bad_gateway(si.custom_error(502));
	verbose(V) << "(execute_cgi) full_program " << full_program << std::endl;
	std::vector<std::string> argv(arg_vec.size() + 1);
	argv[0] = full_program;
	for (size_t i = 1; i < arg_vec.size(); i++)
		if (FileString::exists(root_path + arg_vec[i]))
			argv[i] = root_path + arg_vec[i];
		else
			argv[i] = arg_vec[i];
	verbose(1) << "(webserv:cgi) ";
	for (size_t i = 0; i < argv.size(); i++)
		verbose(1) << argv[i] << " ";
	verbose(1) << std::endl;
	return cgi_pipe(si, argv);
}

int ws_reply_instance::is_cgi_exec(ws_server_instance& si)
{
	int V(1);
	std::vector<std::string> cgi_accept(si.config.get_vector_str("cgi_accept"));
	verbose(V) << "(is_cgi_exec) cgi_accept " << cgi_accept << std::endl;
	std::string call_method = si.in_header.method;
	verbose(V) << "(is_cgi_exec) call_method " << call_method << std::endl;
	bool good_method = false;
	for (size_t i = 0; i < cgi_accept.size(); i++)
		if (cgi_accept[i] == call_method)
			good_method = true;
	if (!good_method)
		return 0;
	DataFold cgi_vec(si.config.get<DataFold>("cgi"));
	verbose(V) << "(is_cgi_exec) cgi_vec " << cgi_vec << std::endl;
	verbose(V) << "(is_cgi_exec) .empty() " << cgi_vec.empty() << std::endl;
	if (cgi_vec.empty())
		return 0;
	while (cgi_vec.loop())
	{
		std::vector<std::string> cgi_params(StringTools::split(cgi_vec.val, " "));
		std::string cgi_extension = cgi_params[0];
		verbose(V) << "(is_cgi_exec) cgi_extension " << cgi_extension << std::endl;
		std::string call_extension = StringTools::get_file_extension(si.in_header.directory);
		verbose(V) << "(is_cgi_exec) call_extension " << call_extension << std::endl;
		if (cgi_extension != call_extension)
			continue ;
		std::string cgi_children = cgi_vec.val.substr(cgi_extension.length() + 1);
		verbose(V) << "(is_cgi_exec) cgi_children " << cgi_children << std::endl;
		si.cgi_flag = true;
		return execute_cgi(si, cgi_children);
	}
	si.cgi_flag = false;
	return 0;
}

bool ws_server_instance::is_cgi() const
{ return cgi_flag; }
