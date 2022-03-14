/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileString.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 09:30:53 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/14 19:51:24 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileString.hpp"
#include <iostream>
#include <algorithm>

FileString::FileString()
: _read_ok(false), _processed_ok(false) {}

void FileString::load(const char * file_name)
{
	_file_name = const_cast<char *>(file_name);
	std::fstream file_read;
	file_read.open(_file_name, std::ios::in);
	if (!file_read)
	{
		_read_ok = false;
		file_read.close();
		return ;
	}
	else
		_read_ok = true;
	std::string line;
	while (std::getline(file_read, line))
		_content += line + "\n";
	parse();
	_processed_ok = true;
}

FileString::FileString(const char * file_name)
: _content(""), _processed(""),
  _read_ok(false), _processed_ok(false) 
{
	load(file_name);
}

std::string const FileString::operator[](std::string key) const
{ return fs_data[key]; }

void FileString::parse()
{
	_processed = _content;
	remove_comments(_processed);
	soft_trim(_processed);
	hard_trim(_processed);
	_processed_ok = true;
	this->fs_data = DataFold(_processed);
}

void FileString::parse(std::string str)
{
	_content = str;
	_processed = _content;
	remove_comments(_processed);
	soft_trim(_processed);
	hard_trim(_processed);
	_processed_ok = true;
	this->fs_data = DataFold(_processed); // Future: implement +=
}

FileString::FileString(FileString & src)
: StringTools()
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
		this->_processed_ok = rhs.isProcessed();
		this->_processed = rhs.getProcessed();
	}
	return *this;
}

bool FileString::isProcessed()
{ return _processed_ok; }

char * FileString::getFileName()
{ return _file_name; }

std::ostream & operator<< (std::ostream & o, FileString const & self)
{
//	o << ">>>" << self.getProcessed() << "<<<" << std::endl;
 	o << self.getDataFold() << std::endl;
	return o;
}

FileString::~FileString()
{ return ; }

DataFold FileString::getDataFold() const
{ return fs_data; }

std::string FileString::getProcessed()
{ return _processed_ok ? _processed : 0; }

std::string FileString::getProcessed() const
{ return _processed_ok ? _processed : 0; }

std::string FileString::getContent()
{ return _content; }

bool FileString::fail()
{ return !_read_ok; }

bool FileString::success()
{ return _read_ok; }
