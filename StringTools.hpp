/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringTools.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/07 01:43:08 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/16 17:09:16 by fde-capu         ###   ########.fr       */
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
		size_t find_outside_quotes(std::string&) const;
		size_t find_closing_bracket(std::string) const;
		size_t find_outside_quotes_set(std::string& str, std::string set) const;
		void remove_comments(std::string&) const;
		std::string correct_quotes(std::string&) const;
		std::string correct_quotes(const std::string&) const;
		std::string escape_char(const std::string, std::string) const;
		bool isNumber(std::string) const;
		bool isAllNumber(std::vector<std::string>) const;
		bool isDigit(char) const;
		std::string itos(int) const;
		std::string apply_quotes(std::string) const;
		std::string apply_quotes(std::string, std::string) const;
		std::vector<std::string> splitOutsideQuotes(std::string) const;
		std::string itoa(int) const;
		bool valid_file_name(std::string) const;
		bool isAllInSet(std::string, std::string) const;
};

std::ostream & operator<< (std::ostream & o, StringTools const & i);

#endif
