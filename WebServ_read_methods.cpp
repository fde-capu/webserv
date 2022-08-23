/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ_read_methods.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/04 15:35:04 by fde-capu          #+#    #+#             */
/*   Updated: 2022/08/23 16:02:54 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

std::string WebServ::get_raw_data(int fd)
{
	static int V(4);
	static size_t PRINT_LIMIT(80);
	CircularBuffer buffer(fd);

	buffer.receive_until_eof();

	verbose(V) << "(get_raw_data) EOF? " << buffer.ended() << std::endl;

	std::string raw_data(buffer.output);

	if (raw_data.length() > PRINT_LIMIT)
	{
		verbose(V) << "(get_raw_data) RAW_DATA >>" << \
			std::string(raw_data.substr(0, PRINT_LIMIT)) << "...<< len(" << \
			raw_data.length() << ")" << std::endl;
	}
	else
		verbose(V) << "(get_raw_data) RAW_DATA >>" << raw_data << "<<" << std::endl;

	return raw_data;
}

int ws_server_instance::read_more_general()
{
	if (is_chunked() && is_multipart())
		return 422;
	if (!is_chunked() && !is_multipart())
		read_more_plain(max_size);
	if (is_chunked())
		read_more_chunked();
	if (is_multipart())
	{
		if (in_header.content_length > CIRCULARBUFFER_LIMIT)
			return 507;
		read_more_multipart();
	}
	return status;
}

bool ws_server_instance::check_socket_stream(CircularBuffer& buf)
{
	set_sizes();
	insufficient_resources = buf.fail();
	return \
		!exceeded_limit && \
		!reached_limit && \
		!buf.ended() && \
		!insufficient_resources;
}

void ws_server_instance::read_more_plain(const size_t& max)
{
	static int V(1);
	size_t next_load;
	CircularBuffer buf(fd);

	if (in_header.method == "GET")
		return ;
	while (check_socket_stream(buf))
	{
		if (max <= in_body.length())
			return ;
		next_load = max - in_body.length();
		verbose(V) << "(read_more_plain) max " << max << std::endl;
		in_body = buf.receive_at_least(next_load);
	}
}

void ws_server_instance::read_more_multipart()
{
	static int V(1);
	verbose(V) << "(read_more_multipart) Calling read_more_plain." << std::endl;
	read_more_plain(in_header.content_length);
	mount_multipart();
}

void ws_server_instance::read_more_chunked()
{
	static int V(1);
	verbose(V) << "(read_more_chunked) Calling read_more_plain." << \
		std::endl;
	verbose(V) << "(read_more_chunked) max_size " << max_size << std::endl;

	read_more_plain(max_size);
	chunked_content = in_body;
	set_sizes();
	if (exceeded_limit)
		status = 413;

	verbose(V) << "(read_more_chunked) Finished with body " \
		<< in_body.length() << " and chunked-content " \
		<< chunked_content.length() << "." << std::endl;
}
