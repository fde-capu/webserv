/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ_read_methods.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/04 15:35:04 by fde-capu          #+#    #+#             */
/*   Updated: 2022/08/11 13:07:43 by fde-capu         ###   ########.fr       */
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
		read_more_plain();
	if (is_chunked())
		read_more_chunked();
	if (is_multipart())
		read_more_multipart();
	return 0;
}

void ws_server_instance::read_more_plain()
{
	static int V(1);
	size_t next_load;
	CircularBuffer buf(fd);

	if (in_header.method == "GET" || in_body.length() > max_size)
		return ;
	do
	{
		next_load = max_size - in_body.length();
		in_body = buf.receive_at_most(next_load);
		set_sizes();
		if (exceeded_limit)
		{
			verbose(V) << "(read_more_plain) Multipart content exceeded limit." \
				<< std::endl;
			break ;
		}
		if (buf.ended())
		{
			verbose(V) << "(read_more_plain) No more data." << std::endl;
			break ;
		}
	}
	while (buf.length() < next_load);

	verbose(V) << "(read_more_plain) Finished with body " << in_body.length() << \
		" and multipart-content " << multipart_content.length() << "." << \
		std::endl;
}

void ws_server_instance::read_more_multipart()
{
	static int V(1);
	size_t next_load;
	CircularBuffer buf(fd);

	set_sizes();
	while (!exceeded_limit && !buf.ended())
	{
		next_load = in_header.content_length - in_body.length();
		verbose(V) << "(read_more_multipart) next_load: " << next_load << std::endl;
		in_body.append(buf.receive_exactly(next_load));
		set_sizes();
	}
	insufficient_resources = buf.fail();
	mount_multipart();

	verbose(V) << "(read_more_multipart) Finished with body " << in_body.length() << \
		" and multipart-content " << multipart_content.length() << "." << \
		std::endl;
}

void ws_server_instance::read_more_chunked()
{
	static int V(1);
	size_t next_load;
	CircularBuffer buf(fd);

	set_sizes();
	while (!exceeded_limit && !buf.ended())
	{
		next_load = in_header.content_length - in_body.length();
		verbose(V) << "(read_more_chunked) next_load: " << next_load << std::endl;
		in_body.append(buf.receive_exactly(next_load));
		set_sizes();
	}
	insufficient_resources = buf.fail();
	mount_chunked();

	verbose(V) << "(read_more_chunked) Finished with body " << in_body.length() << \
		" and multipart-content " << multipart_content.length() << "." << \
		std::endl;
}
