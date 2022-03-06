/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DataFold.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/06 18:45:14 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/06 18:45:19 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DataFold.hpp"
#include <iostream>

DataFold::DataFold(void)
{
	return ;
}

DataFold::DataFold(DataFold const & src)
{
	*this = src;
	return ;
}

DataFold & DataFold::operator= (DataFold const & rhs)
{
	if (this != &rhs)
	{
//		this->member = rhs.getMember();
	}
	return *this;
}

std::ostream & operator<< (std::ostream & o, DataFold const & self)
{
	o << "::DataFold::" << std::endl;
	(void)self;
	return o;
}

DataFold::~DataFold(void)
{
	return ;
}
