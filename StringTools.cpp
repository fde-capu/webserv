/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringTools.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/07 01:42:53 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/07 02:36:42 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "StringTools.hpp"
#include <iostream>

StringTools::StringTools(void)
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
	(void)self;
	return o;
}

StringTools::~StringTools(void)
{
	return ;
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

void StringTools::substitute_all(std::string& dst, std::string before, std::string after)
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

size_t StringTools::find_outside_quotes(std::string &dst)
{ return find_outside_quotes(dst, _quote_set); }

size_t StringTools::find_outside_quotes(std::string& str, std::string quotes_set)
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

std::string StringTools::escape_char(std::string& dst, char esc)
{
	(void)esc;
	return dst;
}

std::string StringTools::correct_quotes(std::string& dst)
{
	std::cout << "###########" << dst << std::endl;
	std::string::iterator f = dst.begin();
	std::string::iterator l = dst.end() - 1;
	std::string first(f, f + 1);
	std::string last(l, l + 1);
	if (first == last && (first == "\"" || first == "\'"))
		return std::string(f + 1, l);

	std::cout << std::string(f, f + 1) << " - " << std::string(l, l + 1);
	std::cout << std::endl;
	return dst;
}

