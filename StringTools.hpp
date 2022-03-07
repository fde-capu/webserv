/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringTools.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/07 01:43:08 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/07 03:40:54 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRINGTOOLS_HPP
# define STRINGTOOLS_HPP

# include <iostream>
# include <sstream>

# define ST_SOFT_TRIM " \t\n"
# define ST_HARD_TRIM " \t"
# define ST_QUOTE_SET "\"'`"
# define ST_COMMENT_INLINE "#"

class StringTools
{
	public:
		StringTools(void);
		StringTools(StringTools const & src);
		StringTools & operator= (StringTools const & rhs);
		~StringTools(void);
		std::string _soft_trim_set;
		std::string _hard_trim_set;
		std::string _quote_set;
		std::string _comment_open;
		std::string _comment_close;
		std::string _comment_inline;
		void soft_trim(std::string&);
		void soft_trim(std::string&, std::string);
		void hard_trim(std::string&);
		void hard_trim(std::string&, std::string);
		void remove_all(std::string&, std::string);
		std::string substitute_all(std::string& dst, std::string before, std::string after);
		void erase_boundaries(std::string &, std::string);
		void erase_boundaries(std::string &dst, std::string center, std::string trim_set);
		size_t find_outside_quotes(std::string&, std::string);
		size_t find_outside_quotes(std::string&);
		void remove_comments(std::string&);
		std::string correct_quotes(std::string&);
		std::string escape_char(std::string&, std::string);
		std::string clean_before_parse(std::string &);
		bool isNumber(std::string);
		bool isDigit(char);
};

std::ostream & operator<< (std::ostream & o, StringTools const & i);

#endif
