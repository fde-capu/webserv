/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DataFold.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/06 18:45:14 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/08 20:43:36 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DataFold.hpp"
#include <iostream>

datafold_type::operator std::string()
{
	std::stringstream o;
	o << this;
	return o.str();
}

datavec::operator std::string()
{
	std::stringstream o;
	o << this;
	return o.str();
}

DataFold::operator std::string()
{
	std::stringstream o;
	o << getCore();
	return o.str();
}

std::ostream & operator<< (std::ostream & o, datafold_t const & self)
{
	o << DF_KEY_QUOTE << self.key << DF_KEY_QUOTE << DF_KVDIV;
	if (self.type & DF_TYPE_NUMBER)
		o << DF_NUM_QUOTE << self.val << DF_NUM_QUOTE;
	if (self.type & DF_TYPE_STRING)
	{
		std::string esc_val = StringTools().escape_char(self.val, DF_VAL_QUOTE);
		o << DF_VAL_QUOTE << esc_val << DF_VAL_QUOTE;
	}
	if (self.type & DF_TYPE_SUB)
		o << self.sub;
	return o;
}

std::ostream & operator<< (std::ostream & o, std::vector<datafold_t> const & self)
{
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

std::ostream & operator<< (std::ostream & o, DataFold const & self)
{
	o << self.getCore();
	return o;
}

DataFold::DataFold(void)
: index(0)
{
	return ;
}

DataFold::DataFold(std::string df_data)
{
	*this = parse_data(df_data);
}

DataFold::DataFold(DataFold const & src)
: StringTools()
{
	*this = src;
	return ;
}

datavec DataFold::getCore() const
{ return core; }

int DataFold::getIndex() const
{ return index; }

int DataFold::getInt(std::string key)
{
	int kc = key_count(key);

	if (!kc)
		throw std::invalid_argument(DF_ERR_NO_KEY);
	if (kc != 1)
		throw std::invalid_argument(DF_ERR_IS_ARRAY);
	return std::atoi(get_datafold(key).val.c_str());
}

DataFold & DataFold::operator= (DataFold const & rhs)
{
	if (this != &rhs)
	{
		core = rhs.getCore();
		index = rhs.getIndex();
	}
	return *this;
}

std::string DataFold::quoted_val(datafold_t dt) const
{
	std::stringstream out;
	if (dt.type & DF_TYPE_NUMBER)
		out << apply_quotes(dt.val, DF_NUM_QUOTE);
	if (dt.type & DF_TYPE_STRING)
		out << apply_quotes(dt.val, DF_VAL_QUOTE);
	if (dt.type & DF_TYPE_SUB)
		out << dt.sub;
	return out.str();
}

std::string DataFold::eqvals_to_arrstr(std::string key) const
{
	bool put_comma = false;
	std::string out(DF_ARRAY_INIT);
	for (int i = 0; i < index; i++)
	{
		if (core[i].key == key)
		{
			if (put_comma)
				out += DF_COMMA;
			out += quoted_val(core[i]);
			put_comma = true;
		}
	}
	out += DF_ARRAY_END;
	return out;
}

DataFold::~DataFold(void)
{ return ; }

int DataFold::df_type(std::string val)
{
	int out;
	soft_trim(val);
// For future implementation:
//	if (find_outside_quotes(val, " ") != std::string::npos)
//		return DF_TYPE_ARRAY;
	out = isNumber(val) ? DF_TYPE_NUMBER : DF_TYPE_STRING;
	return out;
}

void DataFold::push_back(std::string key, std::string val)
{
	datafold_t entry;
	entry.index = index++;
	entry.type = df_type(val);
	entry.key = key;
	if (entry.type & DF_TYPE_STRING || entry.type & DF_TYPE_NUMBER)
		entry.val = correct_quotes(val);
	core.push_back(entry);
}

void DataFold::push_back(std::string key, DataFold sub)
{
	datafold_t entry;
	entry.index = index++;
	entry.type = DF_TYPE_SUB;
	entry.key = key;
	entry.sub = sub;
	core.push_back(entry);
}

size_t DataFold::size() const
{ return core.size(); }

const datafold_type DataFold::operator[] (size_t idx) const
{ return core[idx]; }

int DataFold::key_count(std::string key) const
{
	std::cout << core << std::endl;
	int out = 0;
	for (int i = 0; i < index; i++)
	{
		std::cout << core[i] << std::endl;
		out += core[i].key == key ? 1 : 0;
		std::cout << out << ": " << key << " : " << core[i].key << std::endl;
	}
	return out;
}

datafold_t DataFold::get_datafold(std::string key)
{
	for (int i = 0; i < index; i++)
		if (key == core[i].key)
			return core[i];
	return datafold_t();
}

const std::string DataFold::getValStr(std::string key) const
{
	for (int i = 0; i < index; i++)
		if (key == core[i].key)
			return core[i].val;
	return std::string("");
}

const std::string DataFold::operator[] (std::string key) const
{
	int kc = key_count(key);

	if (!kc)
		return std::string("");
	if (kc == 1)
		return getValStr(key);
	return eqvals_to_arrstr(key);
}

DataFold::operator datavec()
{ return core; }

DataFold DataFold::parse_data(const std::string str)
{
	DataFold out;
	std::string ops = str;
	size_t div_pos;
	bool pass;
	std::string key;
	std::string val;
	size_t pos[4];

	clean_before_parse(ops);

	pass = false;
	while (!pass)
	{
		pass = true;
		pos[0] = find_outside_quotes(ops, " ");
		pos[1] = find_outside_quotes(ops, "{");
		pos[2] = find_outside_quotes(ops, ";");
		pos[3] = find_outside_quotes(ops, "\n");

		if (pos[0] < pos[1] && pos[0] != std::string::npos)
		{
			pass = false;
			key = ops.substr(0, pos[0]);
			ops = ops.substr(pos[0] + 1);
			if (pos[2] < pos[3])
				div_pos = find_outside_quotes(ops, ";");
			else
				div_pos = find_outside_quotes(ops, "\n");
			val = ops.substr(0, div_pos);
			ops = ops.substr(div_pos + 1);
			soft_trim(key);
			soft_trim(val);
			out.push_back(key, val);
			continue ;
		}
		if (pos[1] < pos[0] && pos[1] != std::string::npos)
		{
			pass = false;
			key = ops.substr(0, pos[1]);
			ops = ops.substr(pos[1] + 1);
			div_pos = find_outside_quotes(ops, "}");
			val = ops.substr(0, div_pos);
			ops = ops.substr(div_pos + 1);
			soft_trim(key);
			soft_trim(val);
			out.push_back(key, parse_data(val));
			continue ;
		}
	}
	return out;
}
