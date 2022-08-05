/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ_read_methods.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/04 15:35:04 by fde-capu          #+#    #+#             */
/*   Updated: 2022/08/05 12:48:38 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

std::string WebServ::get_raw_data(int fd)
{
	static size_t PRINT_LIMIT(80);
	static int V(1);
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

void ws_server_instance::read_more_chunked()
{
	static int V(1);
	CircularBuffer buf(fd);
	size_t next_load;

	if (in_header.method == "GET" || chunked_content.length() > max_size)
		return ;

	verbose(V) << "(read_more_chunked) max_size: " << max_size << std::endl;

	do
	{
		verbose(V) << "(read_more_chunked) Will read more chunks." << std::endl;
		next_load = max_size - chunked_content.length();
		in_body = buf.receive_at_most(next_load);
		mount_chunked();
		if (chunked_ended)
		{
			verbose(V) << "(read_more_chunked) End of chunked stream." << std::endl;
			break ;
		}
	}
	while (buf.length() < next_load);

	verbose(V) << "(read_more_chunked) Finished with body " << in_body.length() \
		<< " and chunked_content " << chunked_content.length() << "." \
		<< std::endl;
}


void ws_server_instance::read_more_multipart()
{
	static int V(1);
	size_t next_load;
	CircularBuffer buf(fd);

	if (in_body.length() > static_cast<size_t>(in_header.content_length))
		return ;
	do
	{
		next_load = in_header.content_length - in_body.length();
		in_body = buf.receive_at_most(next_load);
		mount_multipart();
		if (exceeded_limit)
		{
			verbose(V) << "(read_more_multipart) Multipart content exceeded limit." \
				<< std::endl;
			break ;
		}
		if (buf.ended())
		{
			verbose(V) << "(read_more_multipart) No more data." << std::endl;
			break ;
		}
	}
	while (buf.length() < next_load);

	verbose(V) << "(read_more_multipart) Finished with body " << in_body.length() << \
		" and multipart-content " << multipart_content.length() << "." << \
		std::endl;
}
