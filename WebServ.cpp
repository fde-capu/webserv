/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 14:24:28 by fde-capu          #+#    #+#             */
/*   Updated: 2022/04/22 14:31:57 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
#include <iostream>

WebServ::WebServ(DataFold& u_config)
: config(u_config)
{
	return ;
}

WebServ::WebServ(WebServ const & src)
{
	*this = src;
	return ;
}

DataFold WebServ::getConfig() const
{ return config; }

WebServ & WebServ::operator= (WebServ const & rhs)
{
	if (this != &rhs)
	{
		config = rhs.getConfig();
	}
	return *this;
}

std::ostream & operator<< (std::ostream & o, WebServ const & self)
{
	o << "::WebServ::" << std::endl;
	(void)self;
	return o;
}

WebServ::~WebServ()
{
	return ;
}
