/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   datavec.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 11:42:45 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/16 11:44:58 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DataFold.hpp"

datavec::operator std::string() const
{
	std::stringstream o;
	o << this;
	return o.str();
}

std::ostream & operator<< (std::ostream & o, std::vector<datafold_type> const & self)
{
	std::string obj;
	o << DF_OBJ_INIT;
	for (size_t i = 0; i < self.size(); i++)
	{
		o << self[i];
		if (i + 1 < self.size())
			o << DF_COMMA;
	}
	o << DF_OBJ_END;
	return o;
}
