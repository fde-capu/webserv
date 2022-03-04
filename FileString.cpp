/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileString.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 09:30:53 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/04 21:44:16 by fde-capu         ###   ########.fr       */
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
{ erase_boundaries(dst, "\n", trim_set); }

void FileString::erase_boundaries(std::string &dst, std::string center, std::string trim_set)
{
	for(std::string::iterator i = trim_set.begin(); *i; i++)
	{
		std::string nlpv = std::string(i, i + 1) + center;
		std::string nlnx = center + std::string(i, i + 1);
		substitute_all(dst, nlpv, center);
		substitute_all(dst, nlnx, center);
	}
}

void FileString::soft_trim(std::string trim_set)
{ soft_trim(_processed, trim_set); }

void FileString::soft_trim()
{ soft_trim(_processed, _soft_trim); }

void FileString::remove_comments()
{
	std::string line;
	std::string new_p = "";
	std::istringstream ptois(_processed);
	std::string new_line;
	while (std::getline(ptois, line))
	{
		new_line = line.substr(0, line.find(FILESTRING_COMMENT_INLINE)) + "\n";
		if (new_line != "\n")
			new_p += new_line;
	}
	_processed = new_p;
}

void FileString::remove_all(std::string& dst, std::string to_remove)
{ substitute_all(dst, to_remove, ""); }

void FileString::substitute_all(std::string& dst, std::string before, std::string after)
{
	bool pass = false;
	while (!pass)
	{
		pass = true;
		size_t pos = find_outside_quotes(dst, before);
		if (pos != std::string::npos)
		{
			pass = false;
			dst.replace(pos, before.length(), after);
		}
	}
}

vec_pair_str FileString::split_no_quotes(const std::string str, std::string split_set)
{
	std::vector<std::pair<std::string, std::string> > out;
	std::string ops = str;
	size_t val_pos;
	size_t div_pos;
	std::string c;
	bool pass;

	for(std::string::iterator i = split_set.begin(); *i; i++)
	{
		c = std::string(i, i + 1);
		pass = false;
		while (!pass)
		{
			pass = true;
			div_pos = find_outside_quotes(ops, c);
			if (div_pos != std::string::npos)
			{
				pass = false;
				val_pos = find_outside_quotes(ops, " ");
				if (val_pos != std::string::npos)
				{
					std::string key = ops.substr(0, val_pos);
					ops = ops.substr(val_pos + 1);
					div_pos = find_outside_quotes(ops, c);
					std::string val = ops.substr(0, div_pos);
					ops = ops.substr(div_pos + 1);
					soft_trim(key, _soft_trim);
					soft_trim(val, _soft_trim);
					out.push_back(pair_str(key, val));
				}
				else
				{
				}
			}
		}
	}
	for (size_t i = 0; i < out.size(); i++)
	{
		std::cout << "] " << out[i].first << " : " << out[i].second << std::endl;
	}
	return out;
}

void FileString::parse()
{
	std::string parsed(_processed);
	substitute_all(parsed, "\n", " ");
	substitute_all(parsed, "\t", " ");
	hard_trim(parsed, " ");
	erase_boundaries(parsed, ";", _soft_trim);
	erase_boundaries(parsed, "{", _soft_trim);
	erase_boundaries(parsed, "}", _soft_trim);
	vec_pair_str block = split_no_quotes(parsed, ";");
	_processed = parsed;
}

void FileString::process()
{
	_processed = _content;
	remove_comments();
	soft_trim();
	hard_trim();
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
	o << ">>>" << self.processed() << "<<<";
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
