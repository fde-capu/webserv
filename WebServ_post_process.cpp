/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ_post_process.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/15 22:50:52 by fde-capu          #+#    #+#             */
/*   Updated: 2022/10/24 17:14:58 by fde-capu         ###   ########.fr       */
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
		mount_chunked();
	}
	if (is_multipart())
	{
		if (in_header.content_length > CIRCULARBUFFER_LIMIT)
			return 507;
		mount_multipart();
	}
	return status;
}

void ws_server_instance::mount_chunked()
{
	static int V(1);
	std::string chunk_size_hex;
	std::string chunk_extension;
	size_t chunk_size_bytes;

	set_sizes();
	if (in_header.method == "GET")
		return ;
	if (!reached_limit && !exceeded_limit)
	{
		chunk_size_hex = StringTools::consume_until(in_body, "\r\n");
		chunk_extension = StringTools::get_after_first(chunk_size_hex, ";");
		chunk_size_hex = StringTools::get_before_first(chunk_size_hex, ";");
		chunk_size_bytes = StringTools::strhex2size_t(chunk_size_hex);
		if (chunk_size_bytes == 0)
		{
			verbose(V) << "(mount_chunked) Zero mount finished." << std::endl;
			chunk_finished = true;
			return ;
		}
		chunked_content += StringTools::consume_bytes(in_body, chunk_size_bytes);
		StringTools::just_consume_until(in_body, "\r\n");

		verbose(V + 1) << "(mount_chunked) chunked_content " << SHORT(chunked_content) << std::endl;
		verbose(V + 1) << "(mount_chunked) in_body " << SHORT(in_body) << std::endl;
		verbose(V + 1) << "(mount_chunked) chunk_size_hex " << chunk_size_hex << std::endl;
		verbose(V + 1) << "(mount_chunked) chunk_size_bytes " << chunk_size_bytes << std::endl;

		set_sizes();
	}
	else
	{
		chunk_finished = true;
		verbose(V) << "(mount_chunked) Finished with body " \
			<< in_body.length() << " and chunked-content " \
			<< chunked_content.length() << "." << std::endl;
	}
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
	multipart_finished = true;
	set_sizes();
}
