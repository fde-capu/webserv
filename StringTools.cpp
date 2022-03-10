/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringTools.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/07 01:42:53 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/10 19:15:23 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "StringTools.hpp"
#include <iostream>

StringTools::StringTools()
: _soft_trim_set(ST_SOFT_TRIM), _hard_trim_set(ST_HARD_TRIM),
  _quote_set(ST_QUOTE_SET), _comment_inline(ST_COMMENT_INLINE)
{
	return ;
}

StringTools::StringTools(StringTools const & src)
{
	*this = src;
	return ;
}

StringTools & StringTools::operator= (StringTools const & rhs)
{
	if (this != &rhs)
	{
		this->_soft_trim_set = rhs._soft_trim_set;
		this->_hard_trim_set = rhs._hard_trim_set;
		this->_quote_set = rhs._quote_set;
		this->_comment_open = rhs._comment_open;
		this->_comment_close = rhs._comment_close;
		this->_comment_inline = rhs._comment_inline;
	}
	return *this;
}

std::ostream & operator<< (std::ostream & o, StringTools const & self)
{
	o << "::StringTools::" << std::endl;
	static_cast<void>(self);
	return o;
}

StringTools::~StringTools()
{
	return ;
}

std::string StringTools::hard_trim(const std::string &dst)
{
	std::string out(dst);
	hard_trim(out);
	return out;
}

void StringTools::hard_trim(std::string &dst)
{ hard_trim(dst, _hard_trim_set); }

void StringTools::hard_trim(std::string& dst, std::string single_char)
{
	std::string xx = single_char + single_char;
	size_t pos = find_outside_quotes(dst, xx);
	while (pos != std::string::npos)
	{
		dst.replace(pos, 2, single_char);
		pos = find_outside_quotes(dst, xx);
	}
}

void StringTools::soft_trim(std::string& dst, std::string trim_set)
{ erase_boundaries(dst, "\n", trim_set); }

void StringTools::erase_boundaries(std::string &dst, std::string center)
{ erase_boundaries(dst, center, _soft_trim_set); }

void StringTools::erase_boundaries(std::string &dst, std::string center, std::string trim_set)
{
	for(std::string::iterator i = trim_set.begin(); *i; i++)
	{
		std::string nlpv = std::string(i, i + 1) + center;
		std::string nlnx = center + std::string(i, i + 1);
		substitute_all(dst, nlpv, center);
		substitute_all(dst, nlnx, center);
	}
}

void StringTools::soft_trim(std::string& dst)
{ soft_trim(dst, _soft_trim_set); }

void StringTools::remove_comments(std::string& dst)
{
	std::string line;
	std::string new_p = "";
	std::istringstream ptois(dst);
	std::string new_line;
	while (std::getline(ptois, line))
	{
		new_line = line.substr(0, line.find(ST_COMMENT_INLINE)) + "\n";
		if (new_line != "\n")
			new_p += new_line;
	}
	dst = new_p;
}

void StringTools::remove_all(std::string& dst, std::string to_remove)
{ substitute_all(dst, to_remove, ""); }

std::string StringTools::itos(int i) const
{
	std::ostringstream s;
	s << i;
	return std::string(s.str());
}

std::string StringTools::substitute_all_ret(const std::string dst, std::string before, std::string after) const
{
	std::string out(static_cast<std::string>(dst));
	substitute_all(out, before, after);
	return out;
}

std::string StringTools::substitute_all(std::string& dst, std::string before, std::string after) const
{
	bool pass = false;
	while (!pass)
	{
		pass = true;
		size_t pos = find_outside_quotes(dst, before);
		size_t scap_t = find_outside_quotes(dst, std::string("\\" + before));
		if (pos != scap_t + 1 && pos != std::string::npos)
		{
			pass = false;
			dst.replace(pos, before.length(), after);
		}
	}
	return dst;
}

size_t StringTools::find_outside_quotes(std::string &dst) const
{ return find_outside_quotes(dst, _quote_set); }

std::string StringTools::apply_quotes(std::string str) const
{ return apply_quotes(str, ST_DEFAULT_QUOTE); }

std::string StringTools::apply_quotes(std::string str, std::string quote) const
{ return std::string(quote + escape_char(str, quote) + quote); }

size_t StringTools::find_outside_quotes(std::string& str, std::string needle) const
{
	std::string q = "";
	std::string::iterator e = str.end();
	for(std::string::iterator s = str.begin(); *s; s++)
	{
		for(std::string::const_iterator i = _quote_set.begin(); *i; i++)
		{
			if (*i == *s)
			{
				if (*(q.end() - 1) == *i)
					q = q.substr(0, q.length() - 1);
				else if (q.empty())
					q = q + *i;
			}
			else
			{
				if (e - s < static_cast<long>(needle.size()))
					return std::string::npos;
				if (std::string(s, s + (needle.length())) == needle)
				{
					if (q.length() == 0)
						return s - str.begin();
				}
			}
		}
	}
	return std::string::npos;
}

std::string StringTools::escape_char(const std::string dst, std::string esc) const
{
	return substitute_all_ret(dst, esc, std::string("\\" + esc));
}


std::string StringTools::correct_quotes(const std::string& dst)
{
	std::string out = dst;
	correct_quotes(*const_cast<std::string*>(&dst));
	return dst;
}

std::string StringTools::correct_quotes(std::string& dst)
{
	std::string out(dst);
	std::string::iterator f = dst.begin();
	std::string::iterator l = dst.end() - 1;
	std::string first(f, f + 1);
	std::string last(l, l + 1);
	if (first == last && (first == "\"" || first == "'"))
		out = std::string(f + 1, l);
	dst = out;
	return out;
}

std::string StringTools::clean_before_parse(std::string& dst)
{
	remove_comments(dst);
	soft_trim(dst);
	hard_trim(dst);
	hard_trim(dst, "\n");
	substitute_all(dst, "\t", " ");
	hard_trim(dst, " ");
	erase_boundaries(dst, ";");
	erase_boundaries(dst, "{");
	erase_boundaries(dst, "}");
	return dst;
}

bool StringTools::isDigit(char d)
{
	return d >= '0' && d <= '9';
}

std::vector<std::string> StringTools::splitOutsideQuotes(const std::string vecstr)
{
	std::vector<std::string> out;
	std::string spl = correct_quotes(hard_trim(vecstr));
	size_t pos = find_outside_quotes(spl, " ");
	if (pos == std::string::npos)
	{
		out.push_back(correct_quotes(spl));
		return out;
	}
	size_t pre = 0;
	while (pos != std::string::npos)
	{
		out.push_back(correct_quotes(spl.substr(pre, pos)));
		spl = spl.substr(pos + 1);
		pos = find_outside_quotes(spl, " ");
	}
	out.push_back(correct_quotes(spl));
	return out;
}

bool StringTools::isAllNumber(std::vector<std::string> vecstring)
{
	for (size_t i = 0; i < vecstring.size(); i++)
		if (!isNumber(vecstring[i]))
			return false;
	return true;
}

bool StringTools::isNumber(std::string str)
{
	bool dot = false;
	soft_trim(str);
	correct_quotes(str);
	for (size_t i = 0; i < str.size(); i++)
	{
		if (i == 0 && str[0] != '-' && str[0] != '+' && !isDigit(str[0]) && str[0] != '.')
			return false;
		if (str[i] == '.')
		{
			if (dot)
				return false;
			dot = true;
			continue ;
		}
		if (!isDigit(str[i]))
			return false;
	}
	return true;
}

std::string StringTools::itoa(int i)
{
	std::ostringstream ss;
	ss << i;
	return ss.str();
}
