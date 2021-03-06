/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileString.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 09:31:39 by fde-capu          #+#    #+#             */
/*   Updated: 2022/08/01 13:58:58 by fde-capu         ###   ########.fr       */
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

** To get json it used to be like treating as stream:
** std::cout << conf[key] << std::endl;
** ...but this feature currently disabled (commented out).

*/

#ifndef FILESTRING_HPP
# define FILESTRING_HPP

# include "StringTools.hpp"
# include "DataFold.hpp"

# include <fstream>
# include <sys/types.h>
# include <sys/stat.h>

# ifndef VERBOSE
#  define VERBOSE 0
# endif

class FileString
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
		FileString& operator= (FileString&);
		FileString& operator= (const std::string&);
		~FileString();

		operator DataFold() const;

		void load();
		void load(const char *);
		void load(const std::string &);
		void parse();
		void parse(std::string);
		bool fail() const;
		bool success() const;
		bool isProcessed() const;

		char * getFileName() const;
		std::string getContent() const;
		std::string content() const;
		std::string getProcessed() const;
		DataFold getDataFold() const;
		DataFold get_val(std::string) const;
		DataFold get_val(std::string, std::string) const;

		std::string const operator[](std::string) const;

		template <typename T>
		T get(std::string key) const
		{ return fs_data.get<T>(key); }

		template <>
		std::vector<int> get(std::string key) const
		{ return fs_data.get_vector_int(key); }

		template <>
		std::vector<std::string> get(std::string key) const
		{ return fs_data.get_vector_str(key); }

		template <typename T>
		T get(std::string key, std::string sub) const
		{ return fs_data.get_datafold(key, sub); }

		template <>
		std::vector<int> get(std::string key, std::string sub) const
		{ return fs_data.get_vector_int(key, sub); }

		template <>
		std::vector<std::string> get(std::string key, std::string sub) const
		{ return fs_data.get_vector_str(key, sub); }

		bool loop();
		std::string key;
		std::string val;
		int type;
		int key_count(std::string);

		static void write(std::string, std::string&);
		static bool is_dir(const std::string&);
		static bool is_file(const std::string&);
		static bool exists(const std::string&);
		bool exists();
};

std::ostream & operator<< (std::ostream & o, FileString & i);

#endif
