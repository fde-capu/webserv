/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileString.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 09:30:53 by fde-capu          #+#    #+#             */
/*   Updated: 2022/05/27 20:55:07 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileString.hpp"

FileString::operator DataFold() const
{ return getDataFold(); }

FileString::FileString()
: _read_ok(false), _processed_ok(false),
  key(""), val(""), type(0)
{}

void FileString::load(const std::string & str)
{
	_content = str;
	parse();
}

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
}

FileString::FileString(const char * file_name)
: _content(""), _processed(""),
  _read_ok(false), _processed_ok(false),
  key(""), val(""), type(0)
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
	parse();
}

FileString::FileString(FileString & src)
: key(""), val(""), type(0)
{
	*this = src;
	return ;
}

FileString& FileString::operator= (FileString & rhs)
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

FileString& FileString::operator= (const std::string& rhs)
{ 
	this->_file_name = const_cast<char*>(std::string("").c_str());
	this->_content = rhs;
	this->_read_ok = true;
	parse(rhs); // Puts _processed and _processed_ok.
	return *this;
}

bool FileString::isProcessed()
{ return _processed_ok; }

char * FileString::getFileName()
{ return _file_name; }

//std::ostream & operator<< (std::ostream & o, FileString const & self)
//{
// 	o << self.getDataFold() << std::endl;
//	return o;
//}

FileString::~FileString()
{ return ; }

DataFold FileString::get_val(std::string key) const
{ return fs_data.get_val(key); }

DataFold FileString::get_val(std::string key, std::string sub) const
{ return fs_data.get_val(key, sub); }

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

bool FileString::loop()
{
	if (fs_data.loop())
	{
		key = fs_data.key;
		val = fs_data.val;
		type = fs_data.type;
		return true;
	}
	return false;
}

int FileString::key_count(std::string key)
{ return fs_data.key_count(key); }
