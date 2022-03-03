/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileString.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 09:30:53 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/03 18:57:44 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileString.hpp"
#include <iostream>
#include <algorithm>

FileString::FileString(const char * file_name)
: _file_name(const_cast<char *>(file_name)), _content(""), _processed(""),
  _read_ok(true), _processed_ok(false), 
  _exclude_newline(FILESTRING_DEFAULT_X_NL),
  _soft_trim(FILESTRING_SOFT_TRIM), _hard_trim(FILESTRING_HARD_TRIM), _quote_set(FILESTRING_QUOTE_SET),
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

size_t FileString::find_outside_quotes(std::string quotes_set)
{ return find_outside_quotes(_processed, quotes_set); }

size_t FileString::find_outside_quotes(std::string& str, std::string quotes_set)
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
				if (std::string(s, s + (quotes_set.length())) == quotes_set)
				{
					if (q.length() == 0)
						return s - str.begin();
				}
			}
		}
	}
	return std::string::npos;
}

void FileString::hard_trim(std::string trim_set)
{ hard_trim(_processed, trim_set); }

void FileString::hard_trim(std::string& dst, std::string single_char)
{
	std::string xx = single_char + single_char;
	size_t pos = find_outside_quotes(dst, xx);
	while (pos != std::string::npos)
	{
		dst.replace(pos, 2, single_char);
		pos = find_outside_quotes(dst, xx);
	}
}

void FileString::hard_trim()
{
	for(std::string::iterator i = _hard_trim.begin(); *i; i++)
		hard_trim(std::string(i, i + 1));
}

void FileString::soft_trim(std::string& dst, std::string trim_set)
{
	bool pass = false;
	while (!pass)
	{
		pass = true;
		for(std::string::iterator i = trim_set.begin(); *i; i++)
		{
			std::string nlpv = std::string(i, i + 1) + "\n";
			std::string nlnx = "\n" + std::string(i, i + 1);
			size_t pospv = find_outside_quotes(dst, nlpv);
			size_t posnx = find_outside_quotes(dst, nlnx);
			if (pospv != std::string::npos)
			{
				dst.replace(pospv, 2, "\n");
				pass = false;
				continue ;
			}
			if (posnx != std::string::npos)
			{
				dst.replace(posnx, 2, "\n");
				pass = false;
				continue ;
			}
		}
	}
}

void FileString::soft_trim(std::string trim_set)
{ soft_trim(_processed, trim_set); }

void FileString::soft_trim()
{ soft_trim(_processed, _soft_trim); }

void FileString::process()
{
	_processed = _content;
//	remove_comments();
	soft_trim();
	hard_trim();

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
		this->_soft_trim = rhs._soft_trim;
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
	o << self.processed();
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
