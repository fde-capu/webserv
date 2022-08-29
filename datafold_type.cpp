/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   datafold_type.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 11:42:06 by fde-capu          #+#    #+#             */
/*   Updated: 2022/08/29 19:36:15 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "datafold_type.hpp"

datafold_type::datafold_type()
: index(0), type(0), key(""), val(""), sub() {}

datafold_t::operator std::string() const
{
	std::string out = "";
	std::string tmpval;
	std::vector<std::string> spl;

	if (val == "" && type & DF_TYPE_SUB)
		return key + " : " + static_cast<std::string>(sub);
	if (type & DF_TYPE_ARRAY)
	{
		spl = splitOutsideQuotes(val);
		out += key + " : [ ";
		for (size_t i = 0; i < spl.size(); i++)
		{
			out += spl[i];
			if (i + 1 < spl.size())
				out += " , ";
		}
		out += " ] ";
		return out;
	}
	out += key + " : " + val + " ";
	return out;
}

std::string datavec::string() const
{ return *this; }

datavec::operator std::string() const
{
	std::string out = "{ ";
	for (size_t i = 0; i < this->size(); i++)
	{
		out += static_cast<std::string>((*this)[i]) + "; ";
	}
	out += "} ";
	return trim(out);
}

datafold_t::operator int() const
{
	if (type & DF_TYPE_ARRAY)
		throw std::invalid_argument(DF_ERR_IS_ARRAY DF_ERR_CANT_CONVERT_TO_INT);
	if (!(type & DF_TYPE_NUMBER))
		throw std::invalid_argument(DF_ERR_NOT_NUMBER);
	return std::atoi(val.c_str());
}

datavec::datavec() : loop_index(0), type(0) {};

bool datavec::loop()
{
	if (static_cast<size_t>(loop_index) == size())
	{
		loop_index = 0;
		return false;
	}
	key = this->at(loop_index).key;
	if (this->at(loop_index).type & DF_TYPE_SUB)
		val = this->at(loop_index).sub;
	else
		val = this->at(loop_index).val;
	type = this->at(loop_index).type;
	loop_index++;
	return true;
}

void datavec::loop_reset()
{ loop_index = 0; }

bool datavec::not_ended()
{ return !(static_cast<size_t>(loop_index) == size()); }
