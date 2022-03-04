/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileString.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 09:31:39 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/04 20:41:08 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILESTRING_HPP
# define FILESTRING_HPP

# include <iostream>
# include <fstream>
# include <sstream>
# include <map>

# ifndef VERBOSE
#  define VERBOSE 0
# endif

# define FILESTRING_DEFAULT_X_NL true
# define FILESTRING_SOFT_TRIM " \t\n"
# define FILESTRING_HARD_TRIM " \t"
# define FILESTRING_QUOTE_SET "\"'`"
# define FILESTRING_COMMENT_INLINE "#"

class FileString
{
	private:
		FileString(void);
		char * _file_name;
		std::string _content;
		std::string _processed;
		bool _read_ok;
		bool _processed_ok;

	public:
		FileString(const char * file_name);
		FileString(FileString & src);
		FileString & operator= (FileString & rhs);
		~FileString(void);
		std::map<std::string, void*> _map;
		char * getFileName();
		std::string getContent();
		bool fail();
		bool success();
		bool isProcessed();
		bool _exclude_newline;
		std::string _soft_trim;
		std::string _hard_trim;
		std::string _quote_set;
		std::string _comment_open;
		std::string _comment_close;
		std::string _comment_inline;
		std::string processed() const;
		void process();
		void soft_trim();
		void soft_trim(std::string x);
		void soft_trim(std::string& dst, std::string x);
		void hard_trim();
		void hard_trim(std::string x);
		void hard_trim(std::string& dst, std::string x);
		void remove_all(std::string& dst, std::string x);
		void substitute_all(std::string& dst, std::string before, std::string after);
		size_t find_outside_quotes(std::string& str, std::string x);
		size_t find_outside_quotes(std::string x);
		void remove_comments();
		void parse();
};

std::ostream & operator<< (std::ostream & o, FileString const & i);

#endif
