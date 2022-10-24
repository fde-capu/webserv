/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringTools.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/07 01:42:53 by fde-capu          #+#    #+#             */
/*   Updated: 2022/10/24 15:43:19 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "StringTools.hpp"
#include "DataFold.hpp"
#include <iostream>

std::string StringTools::st_word_set(ST_WORD_SET);
std::string StringTools::_quote_set(ST_QUOTE_SET);

StringTools::StringTools()
: _soft_trim_set(ST_SOFT_TRIM), _hard_trim_set(ST_HARD_TRIM),
  _comment_inline(ST_COMMENT_INLINE)
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
		this->_comment_open = rhs._comment_open;
		this->_comment_close = rhs._comment_close;
		this->_comment_inline = rhs._comment_inline;
	}
	return *this;
}

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
		std::string trimmed = std::string(i, i + 1) + std::string(i, i + 1);
		substitute_super(dst, trimmed, std::string(i, i + 1));
	}
}

std::string trim(std::string& dst)
{
	stool.soft_trim(dst);
	return dst;
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

std::string substitute_all_ret(const std::string dst, std::string before, \
	std::string after)
{ return stool.substitute_all_ret(dst, before, after); }

std::string StringTools::substitute_all_ret(const std::string dst, \
	std::string before, std::string after) const
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
		if (pos != std::string::npos && \
			(pos == 0 || (pos != scap_t + 1 && pos > lastp)))
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

size_t StringTools::find_outside_quotes(std::string& str, std::string needle, size_t u_start)
{
	int V(30);
	std::string q("");
	std::string::iterator e;
	std::string::iterator n;
	size_t n_find;

	verbose(V) << "(find_outside_quotes) >" << needle << "< @ " << SHORT(str) << std::endl;
	n_find = str.find(needle, u_start);
	if (n_find == std::string::npos)
		return std::string::npos;
	std::string::iterator s = str.begin() + n_find;
	while (*s)
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
					q += *i;
				break ;
			}
			else
			{
				if (q.length())
					continue ;
				e = s;
				n = needle.begin();
				while (*e && *n && *e == *n)
				{
					e++;
					n++;
					if (!*e && *n)
					{
						return std::string::npos;
					}
					if (!*n)
					{
						return s - str.begin();
					}
				}
			}
		}
		s++;
	}
	return std::string::npos;
}

size_t StringTools::find_outside_quotes_set(std::string& str, std::string set, size_t u_start)
{
	size_t out = std::string::npos;
	for (std::string::iterator i = set.begin(); *i; i++)
	{
		size_t first = find_outside_quotes(str, std::string(i, i + 1), u_start);
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
	if (!ops[pos])
	{
		return std::string::npos;
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

bool isWordContained(const std::string& word, const std::string& big_string)
{ return stool.isWordContained(word, big_string); }

bool StringTools::isWordContained(const std::string& word, const std::string& big_string) const
{ return isWordInWordSet(word, split(big_string, " ")); }

bool isWordInWordSet(std::string w, std::vector<std::string> vstr)
{ return stool.isWordInWordSet(w, vstr); }

bool StringTools::isWordInWordSet(std::string w, std::vector<std::string> vstr) const
{
	for (size_t i = 0; i < vstr.size(); i++)
	{
		if (vstr[i] == w)
			return true;
	}
	return false;
}

bool isBoolStr(std::string w)
{ return stool.isBoolStr(w); }

bool StringTools::isBoolStr(std::string w) const
{ return stool.isWordInWordSet(w, splitOutsideQuotes(ST_BOOL_WORDS)); }

std::vector<std::string> split(const std::string str, const std::string sep)
{ return stool.split(str, sep); }

std::vector<std::string> StringTools::split(const std::string str, const std::string sep)
{
	std::vector<std::string> out;
	std::string crop = str;
	size_t mark;

	mark = find_outside_quotes(crop, sep);
	while (mark != std::string::npos)
	{
		out.push_back(crop.substr(0, mark));
		crop = crop.substr(mark + sep.length());
		mark = find_outside_quotes(crop, sep);
	}
	out.push_back(crop);
	return out;
}

std::vector<std::string> split_trim(const std::string str, const std::string sep)
{ return stool.split_trim(str, sep); }

std::vector<std::string> StringTools::split_trim(const std::string str, const std::string sep) const
{
	std::vector<std::string> out = split(str, sep);
	for (size_t i = 0; i < out.size(); i++)
		::trim(out[i]);
	return out;
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

bool isAllNumber(std::string ustr)
{ return stool.isAllNumber(ustr); }

bool StringTools::isAllNumber(std::string ustr) const
{ return isNumber(ustr); }

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

std::string StringTools::stoa(size_t i)
{
	std::stringstream ss;
	ss << i;
	return ss.str();
}

bool is_int(std::string isi)
{ return stool.is_int(isi); }

bool StringTools::is_int(std::string isi) const
{
	// Currently accepting arbitrarly large overflewd string input.
	int i_test = 0;
	if (isAllNumber(isi) && 1 == std::sscanf(isi.c_str(), "%i", &i_test))
		return true;
	return false;
}

bool is_size_t(std::string fn)
{ return stool.is_size_t(fn); }

bool StringTools::is_size_t(std::string sts) const
{
	// Currently accepting arbitrarly large overflewd string input.
	size_t st_test = 0;
	if (isAllNumber(sts) && 1 == std::sscanf(sts.c_str(), "%zu", &st_test))
		return true;
	return false;
}

std::string StringTools::get_file_extension(const std::string fpath)
{
	std::vector<std::string> e = split(fpath, ".");
	if (e.size() <= 1)
		return "";
	return "." + e[e.size() - 1];
}

bool isFileName(std::string fn)
{ return stool.isFileName(fn); }

bool StringTools::isFileName(std::string fn) const
{ return isAllInSet(fn, ST_FILENAME_SET); }

std::string StringTools::remove_filename_from_path(const std::string fpath)
{
	std::vector<std::string> u_path = split(fpath, "/");
	std::string bpath("");
	for (size_t i = 0; i < u_path.size() - 1; i++)
		bpath += u_path[i] + "/";
	return bpath;
}

bool isUri(std::string fn)
{ return stool.isUri(fn); }

bool StringTools::isUri(std::string fn) const
{ return isAllInSet(fn, ST_URI_SET); }

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

bool StringTools::not_in_word_set(char x)
{ return st_word_set.find(x) != std::string::npos; }

std::string word_from(const std::string& phrase, size_t pos)
{ return stool.word_from(phrase, pos); }

std::string StringTools::word_from(const std::string& phrase, size_t pos) const
{
	std::string word;
	
	while(not_in_word_set(phrase.at(pos)))
		word += phrase.at(pos++);
	return word;
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

size_t word_count(std::string str)
{ return stool.word_count(str); }

size_t StringTools::word_count(std::string str) const
{
	int out = str.length() ? 1 : 0;
	while (find_outside_quotes_set(str, " \n") != std::string::npos && ++out)
		str = str.substr(find_outside_quotes_set(str, " \n") + 1);
	return out;
}

bool is_equal_insensitive(const std::string a, const std::string b)
{ return stool.is_equal_insensitive(a, b); }

bool StringTools::is_equal_insensitive(const std::string a, const std::string b) const
{ return to_lower(a) == to_lower(b); }

std::string to_lower(std::string a)
{ return stool.to_lower(a); }

std::string StringTools::to_lower(std::string str) const
{
	for (size_t i = 0; i < str.length(); i++)
		str[i] = tolower(str[i]);
	return str;
}

std::string to_upper(std::string a)
{ return stool.to_upper(a); }

std::string StringTools::to_upper(std::string str) const
{
	for (size_t i = 0; i < str.length(); i++)
		str[i] = toupper(str[i]);
	return str;
}

void remove_dup_char(std::string& dst, const char c)
{ return stool.remove_dup_char(dst, c); }

void StringTools::remove_dup_char(std::string& dst, const char c) const
{
	std::string before(2, c);
	std::string after(1, c);
	substitute_all(dst, before, after);
}

void remove_rep_char(std::string& dst, const char c)
{ return stool.remove_rep_char(dst, c); }

void StringTools::remove_rep_char(std::string& dst, const char c) const
{
	std::string before(2, c);
	std::string after(1, c);
	substitute_super(dst, before, after);
}

bool StringTools::isWord(const std::string& str, size_t pos)
{ return pos < str.length() && !not_in_word_set(str.at(pos)); }

std::string StringTools::query_for(std::string query, std::string& src)
{
	int V(1);
	size_t start = src.find(query);
	size_t end;
	std::string tmp("");

	verbose(V) << "(query_for) " << query << std::endl;
	if (start == std::string::npos)
		return "";
	while (start < src.length() && !isWord(src, start++)) ;
	if (start == std::string::npos)
		return "";
	end = find_outside_quotes_set(src, ";\r\n", start);
	if (end != std::string::npos)
		tmp = src.substr(start, end - start);
	return trim(tmp, " \"\'");
}

bool StringTools::isInSet(const char& test_char, std::string set)
{
	size_t h = 0;
	while (h < set.length())
	{
		if (set.at(h) == test_char)
			return true;
		h++;
	}
	return false;
}

bool StringTools::startsWith(const std::string& full, const std::string& begining)
{ return full.find(begining) == 0; }

std::string& StringTools::trim(std::string& str, std::string set)
{
	size_t start(0);
	size_t end(str.length() - 1);

	while (start < str.length() && isInSet(str.at(start), set)) start++;
	while (str.length() && isInSet(str.at(end), set)) end--;
	if (start != 0 || end != str.length() - 1)
		str = str.substr(start, end - start + 1);
	return str;
}

std::string StringTools::consume_bytes(std::string& src, size_t bytes)
{
	std::string ret;
	if (bytes >= src.length())
	{
		ret = src;
		src = "";
		return ret;
	}
	ret = src.substr(0, bytes);
	src.erase(0, bytes);
	return ret;
}

void StringTools::just_consume_until(std::string& src, const std::string delim_str)
{
	size_t h(src.find(delim_str));
	if (h == std::string::npos)
	{
		src = "";
		return;
	}
	src.erase(0, h + delim_str.length());
	return;
}

std::string StringTools::consume_until(std::string& src, const std::string delim_str)
{
	std::string ret;
	size_t h(src.find(delim_str));
	if (h == std::string::npos)
	{
		ret = src;
		src = "";
		return ret;
	}
	ret = src.substr(0, h);
	src.erase(0, h + delim_str.length());
	return ret;
}

std::string StringTools::consume_delims(std::string& src, const std::string delim_set)
{
	size_t h(0);
	std::string ret;

	while (src.length() && h < src.length() - 1 && !isInSet(src.at(h), delim_set))
		h++;
	if (h >= src.length() - 1)
	{
		ret = src;
		src = "";
		return ret;
	}
	ret = src.substr(0, h);
	while (src.length() && h < src.length() - 1 && isInSet(src.at(h), delim_set))
		h++;
	src.erase(0, h);
	return ret;
}

std::string StringTools::get_after_until_line(const std::string& src, const std::string phrase)
{
	size_t h(src.find(phrase));
	if (h == std::string::npos)
		return "";
	size_t nl(src.find("\n", h));
	size_t rl(src.find("\r", h));
	size_t b = nl < rl ? nl : rl;
	return src.substr(h + phrase.length(), b - (h + phrase.length()));
}

std::string StringTools::get_after_first(const std::string& src, const std::string phrase)
{
	size_t h(src.find(phrase));
	if (h == std::string::npos)
		return "";
	return src.substr(h + phrase.length());
}

std::string StringTools::get_before_first(const std::string& src, const std::string phrase)
{
	size_t h(src.find(phrase));
	if (h == std::string::npos)
		return src;
	return src.substr(0, h);
}

size_t StringTools::strhex2size_t(const std::string src)
{
	return static_cast<size_t>(strtoul(src.c_str(), NULL, 16));
}

std::vector<char*> StringTools::vecstr2veccharp(const std::vector<std::string>& argv)
{
	std::vector<char*> vec_cp;
	vec_cp.reserve(argv.size() + 1);
	for (size_t i = 0; i < argv.size(); i++)
		vec_cp.push_back(strdup(argv[i].c_str()));
	vec_cp.push_back(NULL);
	return vec_cp;
}
