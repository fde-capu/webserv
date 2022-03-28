/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringTools.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/07 01:42:53 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/28 15:02:37 by fde-capu         ###   ########.fr       */
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

//std::ostream & operator<< (std::ostream & o, StringTools const & self)
//{
//	o << "::StringTools::" << std::endl;
//	static_cast<void>(self);
//	return o;
//}

StringTools::~StringTools()
{
	return ;
}

std::string hard_trim(const std::string &dst)
{ return stool.hard_trim(dst); }

std::string StringTools::hard_trim(const std::string &dst) const
{
	std::string out(dst);
	hard_trim(out);
	return out;
}

void hard_trim(std::string &dst)
{ return stool.hard_trim(dst); }

void StringTools::hard_trim(std::string &dst) const
{ hard_trim(dst, _hard_trim_set); }

void hard_trim(std::string& dst, std::string set)
{ return stool.hard_trim(dst, set); }

void StringTools::hard_trim(std::string& dst, std::string set) const
{
	for (std::string::iterator i = set.begin(); *i; i++)
	{
		std::string xx = std::string(i, i + 1) + std::string(i, i + 1);
		substitute_super(dst, xx, std::string(i, i + 1));
	}
}

void soft_trim(std::string& dst, std::string set)
{ return stool.soft_trim(dst, set); }

void StringTools::soft_trim(std::string& dst, std::string set) const
{
	bool pass = false;

	while (!pass)
	{
		pass = true;
		for (std::string::iterator i = set.begin(); *i; i++)
		{
			if (*dst.begin() == *i)
			{
				dst = dst.substr(1);
				pass = false;
			}
			if (*(dst.end() - 1) == *i)
			{
				dst = dst.substr(0, dst.size() - 1);
				pass = false;
			}
		}
	}
}

void erase_boundaries(std::string &dst, std::string center)
{ return stool.erase_boundaries(dst, center); }

void StringTools::erase_boundaries(std::string &dst, std::string center) const
{
	for (std::string::iterator i = center.begin(); *i; i++)
		erase_boundaries(dst, std::string(i, i + 1), _soft_trim_set);
}

void erase_boundaries(std::string &dst, std::string center, std::string trim_set)
{ return stool.erase_boundaries(dst, center, trim_set); }

void StringTools::erase_boundaries(std::string &dst, std::string center, std::string trim_set) const
{
	for (std::string::iterator i = trim_set.begin(); *i; i++)
	{
		std::string nlpv = std::string(i, i + 1) + center;
		std::string nlnx = center + std::string(i, i + 1);
		substitute_super(dst, nlpv, center);
		substitute_super(dst, nlnx, center);
	}
}

void soft_trim(std::string& dst)
{ return stool.soft_trim(dst); }

void StringTools::soft_trim(std::string& dst) const
{ soft_trim(dst, _soft_trim_set); }

std::string dual_trim(std::string& dst, std::string set)
{ return stool.dual_trim(dst, set); }

std::string StringTools::dual_trim(std::string& dst, std::string set) const
{
	while (*dst.begin() == *set.begin() && *(dst.end() - 1) == *(set.end() - 1))
		dst = dst.substr(1, dst.size() - 2);
	return dst;
}

void remove_comments(std::string& dst)
{ return stool.remove_comments(dst); }

void StringTools::remove_comments(std::string& dst) const
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

void remove_all(std::string& dst, std::string to_remove)
{ return stool.remove_all(dst, to_remove); }

void StringTools::remove_all(std::string& dst, std::string to_remove) const
{ substitute_super(dst, to_remove, ""); }

std::string itos(int i)
{ return stool.itos(i); }

std::string StringTools::itos(int i) const
{
	std::ostringstream s;
	s << i;
	return std::string(s.str());
}

std::string substitute_all_ret(const std::string dst, std::string before, std::string after)
{ return stool.substitute_all_ret(dst, before, after); }

std::string StringTools::substitute_all_ret(const std::string dst, std::string before, std::string after) const
{
	std::string out(static_cast<std::string>(dst));
	out = substitute_all(out, before, after);
	return out;
}

std::string substitute_super(std::string& dst, std::string before, std::string after)
{ return stool.substitute_super(dst, before, after); }

std::string StringTools::substitute_super(std::string& dst, std::string before, std::string after) const
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

std::string substitute_all(std::string& dst, std::string before, std::string after)
{ return stool.substitute_all(dst, before, after); }

std::string StringTools::substitute_all(std::string& dst, std::string before, std::string after) const
{
	bool pass = false;
	size_t lastp = 0;
	while (!pass)
	{
		pass = true;
		size_t pos = find_outside_quotes(dst, before);
		size_t scap_t = find_outside_quotes(dst, std::string("\\" + before));
		if (pos != scap_t + 1 && pos != std::string::npos && pos > lastp)
		{
			pass = false;
			lastp = pos;
			dst.replace(pos, before.length(), after);
		}
	}
	return dst;
}

std::string apply_quotes(std::string str)
{ return stool.apply_quotes(str); }

std::string StringTools::apply_quotes(std::string str) const
{ return apply_quotes(str, ST_DEFAULT_QUOTE); }

std::string apply_quotes(std::string str, std::string quote)
{ return stool.apply_quotes(str, quote); }

std::string StringTools::apply_quotes(std::string str, std::string quote) const
{ return std::string(quote + escape_char(str, quote) + quote); }

size_t find_outside_quotes(std::string str, std::string needle)
{ return stool.find_outside_quotes(str, needle); }

size_t StringTools::find_outside_quotes(std::string& str, std::string needle) const
{
	std::string q = "";
	std::string::iterator e = str.end();
	for(std::string::iterator s = str.begin(); *s; s++)
	{
		if (*s == '\\')
			s += 2;
		if (!*s)
			break ;
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

size_t find_outside_quotes_set(std::string& str, std::string set)
{ return stool.find_outside_quotes_set(str, set); }

size_t StringTools::find_outside_quotes_set(std::string& str, std::string set) const
{
	size_t out = std::string::npos;
	for (std::string::iterator i = set.begin(); *i; i++)
	{
		size_t first = find_outside_quotes(str, std::string(i, i + 1));
		if (first < out)
			out = first;
	}
	return out;
}

size_t find_closing_bracket(std::string ops)
{ return stool.find_closing_bracket(ops); }

size_t StringTools::find_closing_bracket(std::string ops) const
{
	std::string stack("");
	size_t pos = 0;
	while (ops[pos])
	{
		if (ops[pos] == '{')
			stack += "{";
		if (ops[pos] == '}')
		{
			if (stack == "")
				return pos;
			stack = stack.substr(0, stack.size() - 1);
		}
		pos++;
	}
	return pos;
}

std::string escape_char(const std::string dst, std::string esc)
{ return stool.escape_char(dst, esc); }

std::string StringTools::escape_char(const std::string dst, std::string esc) const
{
	return substitute_all_ret(dst, esc, std::string("\\" + esc));
}

std::string remove_quotes(const std::string& dst)
{ return stool.remove_quotes(dst); }

std::string StringTools::remove_quotes(const std::string& dst) const
{
	std::string out = dst;
	remove_quotes(*const_cast<std::string*>(&dst));
	return dst;
}

std::string remove_quotes(std::string& dst)
{ return stool.remove_quotes(dst); }

std::string StringTools::remove_quotes(std::string& dst) const
{
	if (find_outside_quotes(dst, " ") != std::string::npos)
		return dst;
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

bool isDigit(char d)
{ return stool.isDigit(d); }

bool StringTools::isDigit(char d) const
{ return d >= '0' && d <= '9'; }

bool isWord(const std::string w)
{ return stool.isWord(w); }

bool StringTools::isWord(const std::string w) const
{ return isAllInSet(w, ST_WORD_SET); }

bool isWordInWordSet(std::string w, std::vector<std::string> vstr)
{ return stool.isWordInWordSet(w, vstr); }

bool StringTools::isWordInWordSet(std::string w, std::vector<std::string> vstr) const
{
	std::cout << "isWordInWordSet!! " << w << ":" << vstr.size() << std::endl;
	for (size_t i = 0; i < vstr.size(); i++)
	{
		std::cout << "\t\tWW " << w << " x " << vstr[i] << "\n";
		if (vstr[i] == w)
			return true;
	}
	return false;
}

std::vector<std::string> splitOutsideQuotes(const std::string vecstr)
{ return stool.splitOutsideQuotes(vecstr); }

std::vector<std::string> StringTools::splitOutsideQuotes(const std::string vecstr) const
{
	std::vector<std::string> out;
	std::string spl = remove_quotes(hard_trim(vecstr));
	size_t pos = find_outside_quotes(spl, " ");
	if (pos == std::string::npos)
	{
		out.push_back(remove_quotes(spl));
		return out;
	}
	size_t pre = 0;
	while (pos != std::string::npos)
	{
		out.push_back(remove_quotes(spl.substr(pre, pos)));
		spl = spl.substr(pos + 1);
		pos = find_outside_quotes(spl, " ");
	}
	out.push_back(remove_quotes(spl));
	return out;
}

bool isAllNumber(std::vector<std::string> vecstring)
{ return stool.isAllNumber(vecstring); }

bool StringTools::isAllNumber(std::vector<std::string> vecstring) const
{
	for (size_t i = 0; i < vecstring.size(); i++)
		if (!isNumber(vecstring[i]))
			return false;
	return true;
}

bool isNumber(std::string str)
{ return stool.isNumber(str); }

bool StringTools::isNumber(std::string str) const
{
	bool dot = false;
	soft_trim(str);
	remove_quotes(str);
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

std::string itoa(int i)
{ return stool.itoa(i); }

std::string StringTools::itoa(int i) const
{
	std::ostringstream ss;
	ss << i;
	return ss.str();
}

bool valid_file_name(std::string fn)
{ return stool.valid_file_name(fn); }

bool StringTools::valid_file_name(std::string fn) const
{ return isAllInSet(fn, ST_FILENAME_SET); }

bool isAllInSet(std::string str, std::string set)
{ return stool.isAllInSet(str, set); }

bool StringTools::isAllInSet(std::string str, std::string set) const
{
	size_t pass;
	for (size_t i = 0; i < str.size(); i++)
	{
		for (pass = 0; pass < set.size(); pass++)
		{
			if (str[i] == set[pass])
				break ;
		}
		if (pass >= set.size())
			return false;
	}
	return true;
}

std::string nth_word(std::string str, int n)
{ return stool.nth_word(str, n); }

std::string StringTools::nth_word(std::string str, int n) const
{
	int wc = word_count(str);
	if (n > wc)
		return "";
	while (--n >= 1)
	{
		str = str.substr(find_outside_quotes_set(str, " \n") + 1);
	}
	if (word_count(str) != 1)
		str = str.substr(0, find_outside_quotes_set(str, " \n"));
	return str;
}

int word_count(std::string str)
{ return stool.word_count(str); }

int StringTools::word_count(std::string str) const
{
	int out = str.length() ? 1 : 0;
	while (find_outside_quotes_set(str, " \n") != std::string::npos && ++out)
		str = str.substr(find_outside_quotes_set(str, " \n") + 1);
	return out;
}
