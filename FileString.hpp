/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileString.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 09:31:39 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/10 18:09:12 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*

** FileString uses DataFold to manage json-like data.

** FileString conf;
** conf.load(file_name);
** conf.load(another_file); // Will complement previous load.

** Get values/vectors directly:
** std::string value = conf.get<std::string>(key);
** int value = conf.get<int>(key);
** std::vector<std::string> value = conf.get<std::vector<std::string> >(key);
** std::vector<int> value = conf.get<std::vector<int> >(key);

** To get json or treat it as stream:
** std::cout << conf[key] << std::endl;

*/

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
		char * _file_name;
		std::string _content;
		std::string _processed;
		bool _read_ok;
		bool _processed_ok;

	protected:
		DataFold fs_data;

	public:
		FileString();
		FileString(const char *);
		FileString(FileString & src);
		FileString & operator= (FileString & rhs);
		~FileString();

		void load(const char *);
		void parse();
		bool fail();
		bool success();
		bool isProcessed();

		char * getFileName();
		std::string getContent();
		std::string getProcessed();
		std::string getProcessed() const;

		std::string const operator[](std::string) const;

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
