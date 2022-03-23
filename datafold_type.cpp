/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   datafold_type.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 11:42:06 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/23 11:28:19 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "datafold_type.hpp"

datafold_t::operator std::string() const
{
	if (type & DF_TYPE_SUB)
	{
		sstr o;
//		o << this->sub;
		o << "(str)datafold_type;";
		return o.str();
	}
	return val;
}

datavec::operator std::string() const
{
	sstr o;
//	o << *this;
	o << "(str)datavec;";
	return o.str();
}

datafold_t::operator int() const
{
	if (type & DF_TYPE_ARRAY)
		throw bad_arg(DF_ERR_IS_ARRAY DF_ERR_CANT_CONVERT_TO_INT);
	if (!(type & DF_TYPE_NUMBER))
		throw bad_arg(DF_ERR_NOT_NUMBER);
	return std::atoi(val.c_str());
}

void datafold_t::log_self() const
{
	xo << "index:\t" << index << nl;
	xo << "type:\t" << type << nl;
	xo << "key:\t" << key << nl;
	xo << "val:\t" << val << nl;
//	xo << "sub:\t" << sub << nl;
	xo << "sub:\t" << "temp_out" << nl;
}

//ostr& operator<< (ostr& o, datafold_type const & self)
//{
//	if (!(self.type & DF_TYPE_ONLY_VAL))
//		o << DF_KEY_QUOTE << self.key << DF_KEY_QUOTE << DF_KVDIV;
//	if (self.type & DF_TYPE_SUB)
//	{
//		o << self.sub;
//		return o;
//	}
//	if (self.type & DF_TYPE_ARRAY && !(self.type & DF_TYPE_ONLY_VAL))
//	{
//		std::string a_val = self.val;
//		o << DF_ARRAY_INIT;
//		substitute_all(a_val, " ", " , ");
////		if ((self.type & DF_TYPE_STRING)
////			&& (!(isNumber(a_val))))
////		{
////			a_val = "'" + escape_char(a_val, "'") + "'";
////		}
//		o << a_val;
//		o << DF_ARRAY_END;
//		return o;
//	}
//	if (self.type & DF_TYPE_NUMBER)
//		o << DF_NUM_QUOTE << self.val << DF_NUM_QUOTE;
//	if (self.type & DF_TYPE_STRING)
//	{
//		if (isNumber(self.val))
//		{
//			o << self.val;
//		}
//		else
//		{
//			std::string esc_val = escape_char(self.val, DF_VAL_QUOTE);
//			o << DF_VAL_QUOTE << esc_val << DF_VAL_QUOTE;
//		}
//	}
//	return o;
//}

datavec::datavec() : loop_index(0) {};

//ostr& operator<< (ostr& o, datavec const& self)
//{
//	std::string obj;
//
//	if (!self.size())
//		return o;
//	datavec tmp_self = *const_cast<datavec*>(&self);
//	if (tmp_self[0].type & DF_TYPE_ONLY_VAL)
//	{
//		o << DF_ARRAY_INIT;
//		while (tmp_self.loop())
//		{
//			if (tmp_self.type & DF_TYPE_STRING && !isNumber(tmp_self.val))
//			{
//				std::string escaped = escape_char(tmp_self.val, "'");
//				o << "'" << escaped << "'";
//			}
//			else
//				o << tmp_self.val;
//			if (tmp_self.not_ended())
//				o << DF_COMMA;
//		}
//		o << DF_ARRAY_END;
//	}
//	else
//	{
//		o << DF_OBJ_INIT;
//		for (size_t i = 0; i < self.size(); i++)
//		{
//			o << self[i];
//			if (i + 1 < self.size())
//				o << DF_DOTCOMMA;
//		}
//		o << DF_OBJ_END;
//	}
//	return o;
//}

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
