/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileString.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 09:30:53 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/07 02:47:13 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileString.hpp"
#include <iostream>
#include <algorithm>

FileString::FileString(const char * file_name)
: StringTools(),
  _file_name(const_cast<char *>(file_name)), _content(""), _processed(""),
  _read_ok(true), _processed_ok(false) 
{
	std::fstream file_read;
	file_read.open(_file_name, std::ios::in);
	if (!file_read)
	{
		_read_ok = false;
		file_read.close();
		return ;
	}
	std::string line;
	while (std::getline(file_read, line))
		_content += line + "\n";
}

void FileString::parse()
{
	std::string parsed(_processed);
	substitute_all(parsed, "\n", " ");
	substitute_all(parsed, "\t", " ");
	hard_trim(parsed, " ");
	erase_boundaries(parsed, ";");
	erase_boundaries(parsed, "{");
	erase_boundaries(parsed, "}");
	this->data = DataFold(parsed);
//	_processed = parsed;
}

void FileString::process()
{
	_processed = _content;
	remove_comments(_processed);
	soft_trim(_processed);
	hard_trim(_processed);
	parse();
//	hard_trim("#");
	_processed_ok = true;
}

std::string FileString::processed() const
{ return _processed; }

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
		this->_content = rhs.getContent();
		this->_read_ok = rhs.success();
		this->_processed = rhs.isProcessed();
	}
	return *this;
}

bool FileString::isProcessed()
{ return _processed_ok; }

char * FileString::getFileName()
{ return _file_name; }

std::ostream & operator<< (std::ostream & o, FileString const & self)
{
	o << ">>>" << self.processed() << "<<<" << std::endl;
	o << self.data << std::endl;
	return o;
}

FileString::~FileString(void)
{
	return ;
}

std::string FileString::getContent()
{ return _content; }

bool FileString::fail()
{ return !_read_ok; }

bool FileString::success()
{ return _read_ok; }
