/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   datafold_type.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 11:42:06 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/16 13:16:09 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "datafold_type.hpp"

datafold_t::operator std::string() const
{
	if (type & DF_TYPE_SUB)
	{
		std::stringstream o;
		o << this->sub;
		return o.str();
	}
	return val;
}

datafold_t::operator int() const
{
	if (type & DF_TYPE_ARRAY)
		throw std::invalid_argument(DF_ERR_IS_ARRAY);
	if (!(type & DF_TYPE_NUMBER))
		throw std::invalid_argument(DF_ERR_NOT_NUMBER);
	return std::atoi(val.c_str());
}

void datafold_t::log_self() const
{
	std::cout << "index:\t" << index << std::endl;
	std::cout << "type:\t" << type << std::endl;
	std::cout << "key:\t" << key << std::endl;
	std::cout << "val:\t" << val << std::endl;
	std::cout << "sub:\t" << sub << std::endl;
}

std::ostream & operator<< (std::ostream & o, datafold_t const & self)
{
	o << DF_KEY_QUOTE << self.key << DF_KEY_QUOTE << DF_KVDIV;
	if (self.type & DF_TYPE_SUB)
	{
		o << self.sub;
		return o;
	}
	if (self.type & DF_TYPE_NUMBER)
		o << DF_NUM_QUOTE << self.val << DF_NUM_QUOTE;
	if (self.type & DF_TYPE_STRING)
	{
		std::string esc_val = StringTools().escape_char(self.val, DF_VAL_QUOTE);
		o << DF_VAL_QUOTE << esc_val << DF_VAL_QUOTE;
	}
	return o;
}
