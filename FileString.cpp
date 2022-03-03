/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileString.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 09:30:53 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/03 19:01:58 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileString.hpp"
#include <iostream>
#include <algorithm>

FileString::FileString(const char * file_name)
: _file_name(const_cast<char *>(file_name)), _content(""), _processed(""),
  _read_ok(true), _processed_ok(false), 
  _exclude_newline(FILESTRING_DEFAULT_X_NL), _trim_set(FILESTRING_TRIMSET),
  _hard_trim(FILESTRING_HARD_TRIM), _quote_set(FILESTRING_QUOTE_SET),
  _comment_open(FILESTRING_COMMENT_OPEN), _comment_close(FILESTRING_COMMENT_CLOSE),
  _comment_inline(FILESTRING_COMMENT_INLINE)
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

size_t FileString::find_outside_quotes(std::string str, std::string x)
{
	std::string q = "";
	for(std::string::iterator s = str.begin(); *s; s++)
	{
		for(std::string::iterator i = _quote_set.begin(); *i; i++)
		{
			if (*i == *s)
			{
				if (*(q.end() - 1) == *i)
					q = q.substr(0, q.length() - 1);
				else
					q += *s;
			}
			else
			{
				if (std::string(s, s + (x.length())) == x)
				{
					if (q.length() == 0)
						return s - str.begin();
				}
			}
		}
	}
	return std::string::npos;
}

void FileString::hard_trim(std::string x)
{
	std::string xx = x + x;
	size_t pos = find_outside_quotes(_processed, xx);
	while (pos != std::string::npos)
	{
		_processed.replace(pos, 2, x);
		pos = find_outside_quotes(_processed, xx);
	}
}

void FileString::process()
{
	_processed = _content;
	for(std::string::iterator i = _hard_trim.begin(); *i; i++)
	{
		hard_trim(std::string(i, i + 1));
	}

//	hard_trim("#");
	_processed_ok = true;
}

std::string FileString::processed()
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
		this->_trim_set = rhs._trim_set;
		this->_hard_trim = rhs._hard_trim;
		this->_quote_set = rhs._quote_set;
		this->_comment_open = rhs._comment_open;
		this->_comment_close = rhs._comment_close;
		this->_comment_inline = rhs._comment_inline;
	}
	return *this;
}

bool FileString::isProcessed()
{ return _processed_ok; }

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
{ return _content; }

bool FileString::fail()
{ return !_read_ok; }

bool FileString::success()
{ return _read_ok; }
