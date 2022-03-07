/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DataFold.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/06 18:45:14 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/07 02:52:46 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DataFold.hpp"
#include <iostream>

DataFold::DataFold(void)
: StringTools(), index(0)
{
	return ;
}

DataFold::DataFold(std::string data)
{
	*this = parse_data(data, ";");
}

DataFold::DataFold(DataFold const & src)
{
	*this = src;
	return ;
}

std::vector<datafold_t> DataFold::getCore() const
{ return core; }

int DataFold::getIndex() const
{ return index; }

DataFold & DataFold::operator= (DataFold const & rhs)
{
	if (this != &rhs)
	{
		core = rhs.getCore();
		index = rhs.getIndex();
	}
	return *this;
}

std::ostream & operator<< (std::ostream & o, datafold_t const & self)
{
//	o << "{ ";;
//	o << "index: " << self.index << ", ";
	o << "'" << self.key << "' : ";
	if (self.type & DF_TYPE_NUMBER)
		o << "" << self.val << "";
	if (self.type & DF_TYPE_STRING)
		o << "'" << self.val << "'";
	if (self.type & DF_TYPE_SUB)
		o << self.sub;
//	o << " , ";
	return o;
}

std::ostream & operator<< (std::ostream & o, std::vector<datafold_t> const & self)
{
	o << "{ ";
	for (size_t i = 0; i < self.size(); i++)
	{
		o << self[i];
		if (i + 1 < self.size())
			o << " , ";
	}
	o << " } ";
	return o;
}

std::ostream & operator<< (std::ostream & o, DataFold const & self)
{
	o << self.getCore();
	return o;
}

DataFold::~DataFold(void)
{
	return ;
}

void DataFold::push_back(std::string key, std::string val)
{
	datafold_t entry;
	entry.index = index++;
	entry.type = DF_TYPE_STRING;
	entry.key = key;
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

DataFold::operator DataFold::datavec()
{ return core; }

DataFold DataFold::parse_data(const std::string str, std::string split_set)
{
	DataFold out;
	std::string ops = str;
	size_t div_pos;
	std::string c;
	bool pass;
	std::string key;
	std::string val;
	size_t pos[2];

	for(std::string::iterator i = split_set.begin(); *i; i++)
	{
		c = std::string(i, i + 1);
		pass = false;
		while (!pass)
		{
			pass = true;
			pos[0] = find_outside_quotes(ops, " ");
			pos[1] = find_outside_quotes(ops, "{");
			if (pos[0] < pos[1] && pos[0] != std::string::npos)
			{
				pass = false;
				key = ops.substr(0, pos[0]);
				ops = ops.substr(pos[0] + 1);
				div_pos = find_outside_quotes(ops, c);
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
				out.push_back(key, parse_data(val, split_set));
				continue ;
			}
		}
	}
	return out;
}

