/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringTools.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/07 01:43:08 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/07 21:30:43 by fde-capu         ###   ########.fr       */
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
# define ST_DEFAULT_QUOTE "'"

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
		std::string substitute_all(std::string&, std::string, std::string) const;
		std::string substitute_all_ret(const std::string, std::string, std::string) const;
		void erase_boundaries(std::string&, std::string);
		void erase_boundaries(std::string&, std::string, std::string);
		size_t find_outside_quotes(std::string&, std::string) const;
		size_t find_outside_quotes(std::string&) const;
		void remove_comments(std::string&);
		std::string correct_quotes(std::string&);
		std::string escape_char(const std::string, std::string) const;
		std::string clean_before_parse(std::string &);
		bool isNumber(std::string);
		bool isDigit(char);
		std::string itos(int) const;
		std::string apply_quotes(std::string) const;
		std::string apply_quotes(std::string, std::string) const;
};

std::ostream & operator<< (std::ostream & o, StringTools const & i);

#endif
