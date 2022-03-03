/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileString.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 09:30:53 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/03 10:08:59 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileString.hpp"
#include <iostream>

FileString::FileString(const char * file_name)
: _file_name(const_cast<char *>(file_name)), _success(true)
{
	std::fstream file_read;
	file_read.open(_file_name, std::ios::in);
	if (!file_read)
	{
		_success = false;
		file_read.close();
		return ;
	}
	std::string line;
	while (std::getline(file_read, line))
	{
		if (VERBOSE)
			std::cout << "[LINE READ] " << line << std::endl;
		content += line;
	}
	if (VERBOSE)
		std::cout << "(final read) " << content << std::endl;
}

FileString::FileString(FileString & src)
{
	*this = src;
	return ;
}

FileString & FileString::operator= (FileString & rhs)
{
	if (this != &rhs)
	{
		this->_file_name = rhs.getFileName();
		this->content = rhs.getContent();
		this->_success = rhs.success();
	}
	return *this;
}

char * FileString::getFileName()
{ return _file_name; }

std::ostream & operator<< (std::ostream & o, FileString const & self)
{
	o << "::FileString::" << std::endl;
	(void)self;
	return o;
}

FileString::~FileString(void)
{
	return ;
}

std::string FileString::getContent()
{ return content; }

bool FileString::fail()
{ return !_success; }

bool FileString::success()
{ return _success; }
