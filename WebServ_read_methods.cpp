/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ_read_methods.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/04 15:35:04 by fde-capu          #+#    #+#             */
/*   Updated: 2022/09/07 03:55:59 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

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

bool ws_server_instance::read_more_plain(const size_t& max)
{
	static int V(1);
	size_t next_load;
	CircularBuffer buf(fd);
	bool data_was_read(false);
	size_t len;

	if (in_header.method == "GET")
		return false;
	while (check_socket_stream(buf) && in_body.length() < max)
	{
		next_load = max - in_body.length();

		verbose(V) << "(read_more_plain) max " << max << " next_load " \
			<< next_load << std::endl;

		len = in_body.length();
		in_body += buf.try_to_receive(next_load);
		data_was_read = data_was_read || len != in_body.length();

		verbose(V) << "in_body " << SHORT(in_body) << std::endl;
	}
	return data_was_read;
}

void ws_server_instance::read_more_chunked()
{
	static int V(1);
	size_t length;
	std::string chunk_size_hex;
	std::string chunk_extension;
	size_t chunk_size_bytes;
	Chronometer chrono;

	while (chrono < 1000)
	{
		if (chrono > 999.9)
			verbose(V) << "(read_more_chunked) chrono: " << chrono << std::endl;
		if (read_more_plain(in_header.content_length))
			chrono.btn_reset();
	}

	verbose(V) << "(read_more_chunked) in_body " << SHORT(in_body) << std::endl;

	if (in_header.method == "GET")
		return ;
	length = 0;
	chunk_size_hex = StringTools::consume_delims_ff(in_body, "\r\n");
	chunk_extension = StringTools::get_after_first(chunk_size_hex, ";");
	chunk_size_hex = StringTools::get_before_first(chunk_size_hex, ";");
	chunk_size_bytes = StringTools::strhex2size_t(chunk_size_hex);
	
	verbose(V) << "(read_more_chunked) chunk_size_hex " << SHORT(chunk_size_hex) << std::endl;
	verbose(V) << "(read_more_chunked) chunk_extension " << SHORT(chunk_extension) << std::endl;
	verbose(V) << "(read_more_chunked) chunk_size_bytes " << chunk_size_bytes << std::endl;

	while (chunk_size_bytes > 0)
	{
		verbose(V) << "(read_more_chunked) in_body " << SHORT(in_body) << std::endl;

		chunked_content += StringTools::consume_delims_ff(in_body, "\r\n");
		length = chunked_content.length();
		chunk_size_hex = StringTools::consume_delims_ff(in_body, "\r\n");
		chunk_size_bytes = StringTools::strhex2size_t(chunk_size_hex);

		verbose(V) << "(read_more_chunked) chunk_size_hex " << SHORT(chunk_size_hex) << std::endl;
		verbose(V) << "(read_more_chunked) chunk_size_bytes " << chunk_size_bytes << std::endl;
		verbose(V) << "(read_more_chunked) length " << length << std::endl;
	}
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

void ws_server_instance::mount_multipart()
{
	set_sizes();
	multipart_content_disposition = StringTools::query_for( \
			"Content-Disposition", in_body);
	multipart_name = StringTools::query_for("name", in_body);
	multipart_filename = StringTools::query_for("filename", in_body);
	multipart_content_type = StringTools::query_for("Content-Type", in_body);
	multipart_content.reserve(body_end - body_start);
	multipart_content = in_body.substr(body_start, body_end - body_start);
	set_sizes();
}

void ws_server_instance::read_more_multipart()
{
	static int V(1);
	verbose(V) << "(read_more_multipart) Calling read_more_plain." << std::endl;
	read_more_plain(in_header.content_length);
	mount_multipart();
}

int ws_reply_instance::read_limits(ws_server_instance& si)
{
	static int V(2);
	static int VPRINTLIM(40);
	int pos_status(0);

	verbose(V) << "(read_limits) max_size: " << si.max_size << "." \
		<< std::endl;
	verbose(V) << "(read_limits) content_type: " << \
		si.in_header.content_type << std::endl;

	pos_status = si.read_more_general();
	si.set_sizes();
	if (si.exceeded_limit)
	{
		set_code(413, "Payload Too Large (Declared Size)");
		out_body = "BODY FOR 413";
		return 413;
	}
	if (pos_status == 507)
	{
		set_code(507, "Insufficient Resources");
		out_body = "BODY FOR 507";
		return 507;
	}
	if (pos_status == 422)
	{
		set_code(422, "Unprocessable Entity");
		out_body = "BODY FOR 422";
		return 422;
	}

	verbose(V) << "(read_limits) in_body >>" << \
		si.in_body.substr(0, VPRINTLIM) << "<< len: " << si.in_body.length() << \
		std::endl;
	verbose(V) << "(read_limits) multipart_content >>" << \
		si.multipart_content.substr(0, VPRINTLIM) << "<< len: " << \
		si.multipart_content.length() << std::endl;
	verbose(V) << "(read_limits) chunked_content >>" << \
		si.chunked_content.substr(0, VPRINTLIM) << "<< len: " << \
		si.chunked_content.length() << std::endl;

	return 0;
}
