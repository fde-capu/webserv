/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ_read_methods.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/04 15:35:04 by fde-capu          #+#    #+#             */
/*   Updated: 2022/08/25 16:49:22 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

void ws_server_instance::read_more_plain(const size_t& max)
{
	static int V(1);
	size_t next_load;
	CircularBuffer buf(fd);

	if (in_header.method == "GET")
		return ;
	while (check_socket_stream(buf) && in_body.length() < max)
	{
		next_load = max - in_body.length();
		verbose(V) << "(read_more_plain) max " << max << " next_load " \
			<< next_load << std::endl;

		verbose(V) << "in_body A " << in_body.substr(0, 100) << std::endl;
		in_body += buf.try_to_receive(next_load);
		verbose(V) << "in_body B " << in_body.substr(0, 100) << std::endl;
	}
}

void ws_server_instance::read_more_chunked()
{
	static int V(1);

	read_more_plain(max_size);

	verbose(1) << "(read_more_chunked)" << std::endl;
	set_sizes();
	if (exceeded_limit)
		status = 413;

	verbose(V) << "(read_more_chunked) Finished with body " \
		<< in_body.length() << " and chunked-content " \
		<< chunked_content.length() << "." << std::endl;
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

void ws_server_instance::read_more_multipart()
{
	static int V(1);
	verbose(V) << "(read_more_multipart) Calling read_more_plain." << std::endl;
	read_more_plain(in_header.content_length);
	mount_multipart();
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
