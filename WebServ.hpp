/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 14:24:08 by fde-capu          #+#    #+#             */
/*   Updated: 2022/04/22 21:33:58 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "DataFold.hpp"
# include <iostream>
# include <vector>
# include <arpa/inet.h>

typedef struct struct_server_instance
{
	std::string current_http_header;
	std::string current_http_body;
	int socket;
	struct sockaddr_in server_address;
	uint16_t port;
} server_instance;

class WebServ
{
	private:
		WebServ();
		DataFold config;
		DataFold server;
		std::vector<server_instance> instance;
		server_instance dftosi(DataFold);

	public:
		WebServ(DataFold&);
		WebServ(WebServ const & src);
		WebServ & operator= (WebServ const & rhs);
		~WebServ();
		DataFold getConfig() const;
		void init();
};

std::ostream & operator<< (std::ostream & o, WebServ const & i);

#endif
