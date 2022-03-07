/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileString.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 09:30:53 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/07 02:27:04 by fde-capu         ###   ########.fr       */
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

DataFold FileString::split_no_quotes(const std::string str, std::string split_set)
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
				out.push_back(key, split_no_quotes(val, split_set));
				continue ;
			}
		}
	}
	return out;
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
	this->data = split_no_quotes(parsed, ";");
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
