/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringTools.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/07 01:43:08 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/10 20:21:32 by fde-capu         ###   ########.fr       */
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
		void soft_trim(std::string&);
		void soft_trim(std::string&, std::string);
		void hard_trim(std::string&);
		std::string hard_trim(const std::string&);
		void hard_trim(std::string&, std::string);
		void remove_all(std::string&, std::string);
		std::string substitute_all(std::string&, std::string, std::string) const;
		std::string substitute_all_ret(const std::string, std::string, std::string) const;
		void erase_boundaries(std::string&, std::string);
		void erase_boundaries(std::string&, std::string, std::string);
		size_t find_outside_quotes(std::string&, std::string) const;
		size_t find_outside_quotes(std::string&) const;
		void remove_comments(std::string&);
		std::string correct_quotes(std::string&);
		std::string correct_quotes(const std::string&);
		std::string escape_char(const std::string, std::string) const;
		std::string clean_before_parse(std::string &);
		bool isNumber(std::string);
		bool isAllNumber(std::vector<std::string>);
		bool isDigit(char);
		std::string itos(int) const;
		std::string apply_quotes(std::string) const;
		std::string apply_quotes(std::string, std::string) const;
		std::vector<std::string> splitOutsideQuotes(std::string);
		std::string itoa(int);
		bool valid_filename(std::string);
		bool isAllInSet(std::string, std::string);
};

std::ostream & operator<< (std::ostream & o, StringTools const & i);

#endif
