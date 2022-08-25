/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ_operator_out.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/04 15:48:34 by fde-capu          #+#    #+#             */
/*   Updated: 2022/08/04 15:50:37 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

std::ostream & operator<< (std::ostream & o, WebServ const & self)
{
	o << "::WebServ::" << std::endl;
	(void)self;
	return o;
}

std::ostream& operator<< (std::ostream & o, ws_server_instance const & wssi)
{
	o << "ws_server_instance | port        | ";
	for (size_t i = 0; i < wssi.port.size(); i++)
	{ o << wssi.port[i] << " "; }
	o << std::endl;
	o << "ws_server_instance | listen_sock | ";
	for (size_t i = 0; i < wssi.listen_sock.size(); i++)
	{ o << wssi.listen_sock[i] << " "; }
	o << std::endl;
	o << "ws_server_instance | config      | " << wssi.config << std::endl;
	o << "ws_server_instance | in_header   :" << std::endl << wssi.in_header \
		<< std::endl;
	if (wssi.in_body.length() < 512)
	{
		o << "ws_server_instance | in_body     :" << std::endl << ">>" << \
			wssi.in_body << "<<" << std::endl;
	}
	else
	{
		o << "ws_server_instance | in_body     :" << std::endl << ">>" << \
			wssi.in_body.substr(0, 500) << "(...)<< len " \
			<< wssi.in_body.length() << "." << std::endl;
	}
	o << "ws_server_instance | multipart_type | " << wssi.multipart_type \
		<< std::endl;
	o << "ws_server_instance | multipart_content_disposition | " << \
		wssi.multipart_content_disposition << std::endl;
	o << "ws_server_instance | multipart_name | " << wssi.multipart_name \
		<< std::endl;
	o << "ws_server_instance | multipart_filename | " \
		<< wssi.multipart_filename << std::endl;
	o << "ws_server_instance | multipart_content_type | " \
		<< wssi.multipart_content_type << std::endl;
	o << "ws_server_instance | boundary >>" << wssi.boundary << "<<" << std::endl;
	o << "ws_server_instance | max_size | " << wssi.max_size << std::endl;
	return o;
}

std::ostream & operator<< (std::ostream & o, ws_header const & wsh)
{
	o << "ws_header | method     | " << wsh.method << std::endl;
	o << "ws_header | directory  | " << wsh.directory << std::endl;
	o << "ws_header | protocol   | " << wsh.protocol << std::endl;
	o << "ws_header | host       | " << wsh.host << std::endl;
	o << "ws_header | port       | " << wsh.port << std::endl;
	o << "ws_header | user_agent | " << wsh.user_agent << std::endl;
	o << "ws_header | accept     | " << wsh.accept << std::endl;
	o << "ws_header | is_valid   | " << wsh.is_valid << std::endl;
	o << "ws_header | status     | " << wsh.status << std::endl;
	o << "ws_header | status_msg | " << wsh.status_msg << std::endl;
	o << "ws_header | connection | " << wsh.connection << std::endl;
	o << "ws_header | con-length | " << wsh.content_length << std::endl;
	o << "ws_header | con-type   | " << wsh.content_type << std::endl;
	o << "ws_header | transf-enc | " << wsh.transfer_encoding << std::endl;
	o << "ws_header | expect     | " << wsh.expect << std::endl;
	return o;
}