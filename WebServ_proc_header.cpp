/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ_proc_header.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/04 15:37:23 by fde-capu          #+#    #+#             */
/*   Updated: 2022/10/13 05:24:51 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

bool WebServ::validate_header_1st_line
	(std::string& test_str, size_t expected_size, bool& is_valid)
{
	std::vector<std::string> carrier;

	carrier = split_trim(test_str, " ");
	is_valid = true;
	return validate_header_entry(carrier, expected_size, is_valid);
}

bool WebServ::validate_header_entry
	(std::vector<std::string>& test, size_t expected_size, bool& is_valid)
{
	is_valid = is_valid && test.size() >= expected_size;
	return is_valid;
}

ws_server_instance::ws_server_instance()
{
	status = 0;
}

ws_header::ws_header()
{
	port = 0;
	is_valid = false;
	status = 0;
	content_length = 0;
}

bool WebServ::read_1st_line(std::string& line, ws_header& header, bool& is_valid)
{
	std::vector<std::string> carrier;

	verbose(4) << "(read_1st_line) >" << line << "<" << std::endl;
	if (validate_header_1st_line(line, 3, is_valid))
	{
		carrier = split_trim(line, " ");
		header.method = carrier[0];
		header.directory = carrier[1];
		header.protocol = carrier[2];
	}
	else
		is_valid = false;
	return is_valid;
}

bool WebServ::host_from_header(std::string& line, ws_header& header, bool& is_valid)
{
	std::vector<std::string> carrier;
	bool prev_valid = is_valid;

	carrier = split_trim(line, ":");
	if (is_equal_insensitive(carrier[0], "host"))
	{
		if (validate_header_entry(carrier, 3, is_valid))
		{
			header.host = carrier[1];
			header.port = atoi(carrier[2].c_str());
		}
		else
		{
			is_valid = prev_valid;
			if (validate_header_entry(carrier, 2, is_valid))
				header.host = carrier[1];
		}
		return is_valid;
	}
	return false;
}

struct ws_header WebServ::get_header(const std::string& full_file)
{
	static int V(1);
	ws_header header;
	std::vector<std::string> line;
	std::vector<std::string> carrier;
	bool is_valid(false);
	size_t header_break;

	header_break = full_file.find("\r\n\r\n");
	if (header_break == std::string::npos)
		return header;
	std::string h_block = full_file.substr(0, header_break);
	line = split_trim(h_block, "\r\n");
	for (size_t i = 0; i < line.size(); i++)
	{
		verbose(V) << "< " << line[i] << std::endl;
		if (ignore_empty(line[i])) continue ;
		if (i == 0 && read_1st_line(line[i], header, is_valid)) continue ;
		if (host_from_header(line[i], header, is_valid)) continue ;
		carrier = split_trim(line[i], ":");
		if (!validate_header_entry(carrier, 2, is_valid)) continue ;
		if (is_equal_insensitive(carrier[0], "user-agent"))
			header.user_agent = carrier[1];
		if (is_equal_insensitive(carrier[0], "accept"))
			header.accept = carrier[1];
		if (is_equal_insensitive(carrier[0], "content-length"))
			header.content_length = atoi(carrier[1].c_str());
		if (is_equal_insensitive(carrier[0], "content-type"))
			header.content_type = carrier[1];
		if (is_equal_insensitive(carrier[0], "transfer-encoding"))
			header.transfer_encoding = carrier[1];
		if (is_equal_insensitive(carrier[0], "expect"))
			header.expect = carrier[1];
		if (is_equal_insensitive(carrier[0], "cookie"))
			header.cookie = carrier[1];
	}
	header.is_valid = is_valid;
	verbose(V) << "(get_header) " << header << std::endl;
	return header;
}

bool ws_server_instance::is_multipart() const
{ return in_header.content_type.find("multipart") == 0; }

bool ws_server_instance::is_chunked() const
{ return stool.is_equal_insensitive(in_header.transfer_encoding, "chunked"); }
