/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DataFold.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/06 18:45:14 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/06 21:41:07 by fde-capu         ###   ########.fr       */
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
	o << "{" << std::endl;
	o << "\tindex: " << self.index << "," << std::endl;
	o << "\tkey: " << self.key << "," << std::endl;
	if (self.type & DF_TYPE_STRING)
		o << "\tval: " << self.val << "," << std::endl;
	if (self.type & DF_TYPE_SUB)
		o << "\tsub: " << self.sub << "," << std::endl;
	o << "}" << std::endl;
	return o;
}

std::ostream & operator<< (std::ostream & o, std::vector<datafold_t> const & self)
{
	for (size_t i = 0; i < self.size(); i++)
		o << self[i] << std::endl;
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
	entry.val = val;
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
