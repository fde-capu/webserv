/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringTools.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/07 01:43:08 by fde-capu          #+#    #+#             */
/*   Updated: 2022/09/19 17:07:44 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRINGTOOLS_HPP
# define STRINGTOOLS_HPP

# include <iostream>
# include <sstream>
# include <vector>
# include <cstdio>

# define ST_SOFT_TRIM " \t\n\r"
# define ST_HARD_TRIM " \t"
# define ST_QUOTE_SET "\"'`"
# define ST_COMMENT_INLINE "#"
# define ST_DEFAULT_QUOTE "'"
# define ST_WORD_SET "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789-"
# define ST_FILENAME_SET "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./-_"
# define ST_URI_SET "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./-_:"
# define ST_BOOL_WORDS "0 1 off on false true"

# define ST_ID "(StringTools) "
# define ST_ERR_UNMATCHED_BRACKETS ST_ID " Unmatched bracket."

class StringTools
{
	public:
		static std::string st_word_set;

		StringTools();
		~StringTools();
		StringTools(StringTools const & src);
		StringTools & operator= (StringTools const & rhs);

		std::string					_soft_trim_set;
		std::string					_hard_trim_set;
		static std::string			_quote_set;
		std::string					_comment_open;
		std::string					_comment_close;
		std::string					_comment_inline;
		void						soft_trim(std::string&) const;
		void						soft_trim(std::string&, std::string) const;
		void						hard_trim(std::string&) const;
		std::string					hard_trim(const std::string&) const;
		void						hard_trim(std::string&, std::string) const;
		std::string					dual_trim(std::string&, std::string) const;
		void						remove_all(std::string&, std::string) const;
		std::string					substitute_all(std::string&, std::string, std::string) const;
		std::string					substitute_super(std::string&, std::string, std::string) const;
		std::string					substitute_all_ret(const std::string, std::string, std::string) const;
		void						erase_boundaries(std::string&, std::string) const;
		void						erase_boundaries(std::string&, std::string, std::string) const;
		static size_t				find_outside_quotes(std::string&, std::string);
		static size_t				find_outside_quotes_set(std::string&, std::string);
		size_t						find_closing_bracket(std::string) const;
		void						remove_comments(std::string&) const;
		std::string					remove_quotes(std::string&) const;
		std::string					remove_quotes(const std::string&) const;
		std::string					escape_char(const std::string, std::string) const;
		bool						isNumber(std::string) const;
		bool						isAllNumber(std::string) const;
		bool						isAllNumber(std::vector<std::string>) const;
		bool						isDigit(char) const;
		bool						isWord(const std::string) const;
		static bool					isWord(const std::string&, size_t);
		bool						isWordContained(const std::string&, const std::string&) const;
		bool						isWordInWordSet(std::string, std::vector<std::string>) const;
		bool						isBoolStr(std::string) const;
		std::string					itos(int) const;
		std::string					apply_quotes(std::string) const;
		std::string					apply_quotes(std::string, std::string) const;
		static std::vector<std::string>	split(const std::string, const std::string);
		std::vector<std::string>	split_trim(const std::string, const std::string) const;
		std::vector<std::string>	splitOutsideQuotes(std::string) const;
		std::string					itoa(int) const;
		static std::string			stoa(size_t);
		bool						isFileName(std::string) const;
		static std::string			remove_filename_from_path(const std::string);
		static std::string			get_file_extension(const std::string);
		bool						is_size_t(std::string) const;
		bool						is_int(std::string) const;
		bool						isUri(std::string) const;
		bool						isAllInSet(std::string, std::string) const;
		std::string					nth_word(std::string, int) const;
		static bool					not_in_word_set(char x);
		size_t						word_count(std::string) const;
		std::string					word_from(const std::string&, size_t) const;
		bool						is_equal_insensitive(const std::string, const std::string) const;
		std::string					to_lower(std::string) const;
		void						remove_dup_char(std::string&, const char) const;
		void						remove_rep_char(std::string&, const char) const;
		static std::string			query_for(std::string, std::string&);
		static std::string&			trim(std::string&, std::string);
		static bool					isInSet(const char&, std::string set);
		static bool					startsWith(const std::string&, const std::string&);
		static std::string			consume_delims(std::string&, const std::string);
		static std::string			consume_until(std::string&, const std::string);
		static std::string			consume_bytes(std::string&, size_t);
		static std::string			get_after_first(const std::string&, const std::string);
		static std::string			get_before_first(const std::string&, const std::string);
		static size_t				strhex2size_t(const std::string);
};

//std::ostream & operator<< (std::ostream & o, StringTools const & i);

static const StringTools stool = StringTools();

bool isWordContained(const std::string&, const std::string&);
void remove_dup_char(std::string&, const char);
void remove_rep_char(std::string&, const char);
std::string to_lower(std::string);
bool is_equal_insensitive(const std::string, const std::string);
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
size_t find_closing_bracket(std::string);
void remove_comments(std::string&);
std::string remove_quotes(std::string&);
std::string remove_quotes(const std::string&);
std::string escape_char(const std::string, std::string);
bool isNumber(std::string);
bool isAllNumber(std::vector<std::string>);
bool isAllNumber(std::string);
bool isDigit(char);
bool isWord(const std::string);
bool isWordInWordSet(std::string, std::vector<std::string>);
bool isBoolStr(std::string);
std::string itos(int);
std::string apply_quotes(std::string);
std::string apply_quotes(std::string, std::string);
std::vector<std::string> split_trim(const std::string, const std::string);
std::vector<std::string> splitOutsideQuotes(std::string);
std::string itoa(int);
bool isFileName(std::string);
bool is_size_t(std::string);
bool is_int(std::string);
bool isUri(std::string);
bool isAllInSet(std::string, std::string);
std::string nth_word(std::string, int);
size_t word_count(std::string);
std::string word_from(const std::string&, size_t);

#endif
