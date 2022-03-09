/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileString.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 09:31:39 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/09 17:37:21 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILESTRING_HPP
# define FILESTRING_HPP

# include <iostream>
# include <fstream>
# include <vector>
# include <utility>
# include "DataFold.hpp"
# include "StringTools.hpp"

# ifndef VERBOSE
#  define VERBOSE 0
# endif

class FileString : public StringTools
{
	private:
		FileString(void);
		char * _file_name;
		std::string _content;
		std::string _processed;
		bool _read_ok;
		bool _processed_ok;

	public:
		DataFold fs_data;
		FileString(const char * file_name);
		FileString(FileString & src);
		FileString & operator= (FileString & rhs);
		~FileString(void);
		char * getFileName();
		std::string getContent();
		bool fail();
		bool success();
		bool isProcessed();
		std::string processed() const;
		std::string const operator[](std::string) const;
		void parse();

		template <typename T>
		T get(std::string key)
		{ return fs_data.get_datafold(key); }

		template <typename T>
		T get(std::string key, std::string sub)
		{ return fs_data.get_datafold(key, sub); }

		template <>
		std::vector<int> get(std::string key)
		{ return fs_data.get_vector_int(key); }

		template <>
		std::vector<std::string> get(std::string key)
		{ return fs_data.get_vector_str(key); }
};

std::ostream & operator<< (std::ostream & o, FileString const & i);

#endif
