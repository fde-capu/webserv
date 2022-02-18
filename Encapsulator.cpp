/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Encapsulator.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/06 02:51:27 by fde-capu          #+#    #+#             */
/*   Updated: 2022/02/18 18:44:39 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Encapsulator.hpp"
#include <iostream>

Encapsulator::Encapsulator()
: body()
{ return ; }

Encapsulator::Encapsulator(const char * u_body)
: body(u_body)
{ return ; }

Encapsulator::Encapsulator(Encapsulator const & src)
{
	*this = src;
	return ;
}

Encapsulator & Encapsulator::operator= (Encapsulator const & rhs)
{
	if (this != &rhs)
	{
		body = rhs.getBody();
	}
	return *this;
}

Encapsulator::~Encapsulator()
{
	response.clear();
	return ;
}

const void * Encapsulator::encapsulate()
{
	response.clear();
//	response << STATUS_200 << std::endl;
//	response << "Server:" << SERVER_NAME << std::endl;
	response << "Content-length: " << body.length() << std::endl;
	response << "Connection: close" << std::endl;
	response << std::endl;
	response << body;
	full_body = response.str();
	return reinterpret_cast<const void *>(full_body.c_str());
}

int Encapsulator::full_length()
{ return full_body.length(); }

std::string Encapsulator::getBody() const
{ return body; }