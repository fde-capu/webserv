/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DataFold.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/06 18:45:14 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/07 00:17:43 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DataFold.hpp"
#include <iostream>

DataFold::DataFold(void)
: index(0)
{
	return ;
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

std::string DataFold::correct_quotes(std::string val)
{
	std::cout << "###########" << val << std::endl;
	std::string::iterator f = val.begin();
	std::string::iterator l = val.end() - 1;
	std::string first(f, f + 1);
	std::string last(l, l + 1);
	if (first == last && (first == "\"" || first == "\'"))
		return std::string(f + 1, l);

	std::cout << std::string(f, f + 1) << " - " << std::string(l, l + 1);
	std::cout << std::endl;
	return val;
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
