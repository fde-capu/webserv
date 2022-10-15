/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ_read_methods.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/04 15:35:04 by fde-capu          #+#    #+#             */
/*   Updated: 2022/10/15 21:08:11 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

int ws_server_instance::process_post()
{
	if (is_chunked() && is_multipart())
		return 422;
	if (is_chunked())
	{
		set_sizes();
		if (exceeded_limit)
			return 507;
		if (reached_limit)
			return status;
		read_more_chunked();
	}
	if (is_multipart())
	{
		if (in_header.content_length > CIRCULARBUFFER_LIMIT)
			return 507;
		mount_multipart();
	}
	return status;
}

void ws_server_instance::read_more_chunked()
{
	static int V(2);
	std::string chunk_size_hex;
	std::string chunk_extension;
	size_t chunk_size_bytes(1);
	Chronometer chrono;

	set_sizes();
	if (in_header.method == "GET" || reached_limit || exceeded_limit)
		return ;
//	while (chrono < 1000)
//	{
//		if (chrono > 999.9)
//			verbose(V) << "(read_more_chunked) chrono: " << chrono << std::endl;
//		if (read_more_plain(in_header.content_length))
//			chrono.btn_reset();
//	}
	while (!reached_limit && !exceeded_limit)
	{
		chunk_size_hex = StringTools::consume_until(in_body, "\r\n");
		chunk_extension = StringTools::get_after_first(chunk_size_hex, ";");
		chunk_size_hex = StringTools::get_before_first(chunk_size_hex, ";");
		chunk_size_bytes = StringTools::strhex2size_t(chunk_size_hex);
		if (chunk_size_bytes == 0)
			break ;
		chunked_content += StringTools::consume_bytes(in_body, chunk_size_bytes);
		StringTools::just_consume_until(in_body, "\r\n");

		verbose(V) << "(read_more_chunked) chunked_content " << SHORT(chunked_content) << std::endl;
		verbose(V) << "(read_more_chunked) in_body " << SHORT(in_body) << std::endl;
		verbose(V) << "(read_more_chunked) chunk_size_hex " << chunk_size_hex << std::endl;
		verbose(V) << "(read_more_chunked) chunk_size_bytes " << chunk_size_bytes << std::endl;

		set_sizes();
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

int ws_reply_instance::read_limits(ws_server_instance& si)
{
	static int V(1);
	int pos_status(0);

	verbose(V) << "(read_limits) max_size: " << si.max_size << "." \
		<< std::endl;
	verbose(V) << "(read_limits) content_type: " << \
		si.in_header.content_type << std::endl;

	pos_status = si.process_post();
	if (si.exceeded_limit)
	{
		set_code(413, "Payload Too Large");
		out_body = TemplateError::page(413, si.custom_error(413));
		return 413;
	}
	if (si.in_header.content_length > CIRCULARBUFFER_LIMIT)
	{
		set_code(507, "Insufficient Resources");
		out_body = TemplateError::page(507, si.custom_error(507));
		return 507;
	}
	if (pos_status == 422)
	{
		set_code(422, "Unprocessable Entity");
		out_body = TemplateError::page(422, si.custom_error(422));
		return 422;
	}

	verbose(V) << "(read_limits) in_body >>" << SHORT(si.in_body) << "<<" << std::endl;
	verbose(V) << "(read_limits) multipart_content >>" << SHORT(si.multipart_content) << "<<" << std::endl;
	verbose(V) << "(read_limits) chunked_content >>" << SHORT(si.chunked_content) << "<<" << std::endl;

	return 0;
}
