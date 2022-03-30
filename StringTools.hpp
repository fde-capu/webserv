/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringTools.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/07 01:43:08 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/30 14:16:34 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRINGTOOLS_HPP
# define STRINGTOOLS_HPP

# include <iostream>
# include <sstream>
# include <vector>

# define ST_SOFT_TRIM " \t\n"
# define ST_HARD_TRIM " \t"
# define ST_QUOTE_SET "\"'`"
# define ST_COMMENT_INLINE "#"
# define ST_DEFAULT_QUOTE "'"
# define ST_WORD_SET "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789"
# define ST_FILENAME_SET "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./-_"
# define ST_BOOL_WORDS "0 1 off on false true"

class StringTools
{
	public:
		StringTools();
		~StringTools();
		StringTools(StringTools const & src);
		StringTools & operator= (StringTools const & rhs);

		std::string _soft_trim_set;
		std::string _hard_trim_set;
		std::string _quote_set;
		std::string _comment_open;
		std::string _comment_close;
		std::string _comment_inline;
		void soft_trim(std::string&) const;
		void soft_trim(std::string&, std::string) const;
		void hard_trim(std::string&) const;
		std::string hard_trim(const std::string&) const;
		void hard_trim(std::string&, std::string) const;
		std::string dual_trim(std::string&, std::string) const;
		void remove_all(std::string&, std::string) const;
		std::string substitute_all(std::string&, std::string, std::string) const;
		std::string substitute_super(std::string&, std::string, std::string) const;
		std::string substitute_all_ret(const std::string, std::string, std::string) const;
		void erase_boundaries(std::string&, std::string) const;
		void erase_boundaries(std::string&, std::string, std::string) const;
		size_t find_outside_quotes(std::string&, std::string) const;
		size_t find_closing_bracket(std::string) const;
		size_t find_outside_quotes_set(std::string&, std::string) const;
		void remove_comments(std::string&) const;
		std::string remove_quotes(std::string&) const;
		std::string remove_quotes(const std::string&) const;
		std::string escape_char(const std::string, std::string) const;
		bool isNumber(std::string) const;
		bool isAllNumber(std::vector<std::string>) const;
		bool isDigit(char) const;
		bool isWord(const std::string) const;
		bool isWordInWordSet(std::string, std::vector<std::string>) const;
		bool isBoolStr(std::string) const;
		std::string itos(int) const;
		std::string apply_quotes(std::string) const;
		std::string apply_quotes(std::string, std::string) const;
		std::vector<std::string> splitOutsideQuotes(std::string) const;
		std::string itoa(int) const;
		bool isFileName(std::string) const;
		bool isAllInSet(std::string, std::string) const;
		std::string nth_word(std::string, int) const;
		size_t word_count(std::string) const;
};

//std::ostream & operator<< (std::ostream & o, StringTools const & i);

static const StringTools stool = StringTools();

std::string trim(std::string&);
void soft_trim(std::string&);
void soft_trim(std::string&, std::string);
void hard_trim(std::string&);
std::string hard_trim(const std::string&);
void hard_trim(std::string&, std::string);
std::string dual_trim(std::string&, std::string);
void remove_all(std::string&, std::string);
std::string substitute_all(std::string&, std::string, std::string);
std::string substitute_super(std::string&, std::string, std::string);
std::string substitute_all_ret(const std::string, std::string, std::string);
void erase_boundaries(std::string&, std::string);
void erase_boundaries(std::string&, std::string, std::string);
size_t find_outside_quotes(std::string, std::string);
size_t find_closing_bracket(std::string);
size_t find_outside_quotes_set(std::string& str, std::string set);
void remove_comments(std::string&);
std::string remove_quotes(std::string&);
std::string remove_quotes(const std::string&);
std::string escape_char(const std::string, std::string);
bool isNumber(std::string);
bool isAllNumber(std::vector<std::string>);
bool isDigit(char);
bool isWord(const std::string);
bool isWordInWordSet(std::string, std::vector<std::string>);
bool isBoolStr(std::string);
std::string itos(int);
std::string apply_quotes(std::string);
std::string apply_quotes(std::string, std::string);
std::vector<std::string> splitOutsideQuotes(std::string);
std::string itoa(int);
bool isFileName(std::string);
bool isAllInSet(std::string, std::string);
std::string nth_word(std::string, int);
size_t word_count(std::string);

#endif
