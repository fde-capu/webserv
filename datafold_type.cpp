/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   datafold_type.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 11:42:06 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/18 02:26:13 by fde-capu         ###   ########.fr       */
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
		throw std::invalid_argument(DF_ERR_IS_ARRAY DF_ERR_CANT_CONVERT_TO_INT);
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
	if (self.type & DF_TYPE_ARRAY)
	{
		std::string a_val = self.val;
		StringTools stool;
		o << DF_ARRAY_INIT;
		if (self.type & DF_TYPE_STRING)
		{
			stool.substitute_all(a_val, " ", "' , '");
			a_val = "'" + a_val + "'";
		}
		if (self.type & DF_TYPE_NUMBER)
		{
			stool.substitute_all(a_val, " ", " , ");
		}
		o << a_val;
		o << DF_ARRAY_END;
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

datavec::datavec() : loop_index(0) {};

datavec::operator std::string() const
{
	std::stringstream o;
	o << *this;
	return o.str();
}

std::ostream & operator<< (std::ostream & o, datavec const& self)
{
	if (!self.size())
		return o;
	std::string obj;
	if (VERBOSE > 2)
		std::cout << " [ type " << self[0].type << " ] ";
	datavec tmp_self = *const_cast<datavec*>(&self);
	if (tmp_self[0].type & DF_TYPE_ONLY_VAL)
	{
		o << DF_ARRAY_INIT;
		while (tmp_self.loop())
		{
			o << tmp_self.val;
			if (tmp_self.not_ended())
				o << DF_COMMA;
		}
		o << DF_ARRAY_END;
	}
	else
	{
		o << DF_OBJ_INIT;
		for (size_t i = 0; i < self.size(); i++)
		{
			o << self[i];
			if (i + 1 < self.size())
				o << DF_COMMA;
		}
		o << DF_OBJ_END;
	}
	return o;
}

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
	loop_index++;
	return true;
}

void datavec::loop_reset()
{ loop_index = 0; }

bool datavec::not_ended()
{ return !(static_cast<size_t>(loop_index) == size()); }
