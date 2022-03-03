/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileString.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 09:31:39 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/03 10:06:53 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILESTRING_HPP
# define FILESTRING_HPP

# ifndef VERBOSE
#  define VERBOSE 0
# endif

# include <iostream>
# include <fstream>

class FileString
{
	private:
		char * _file_name;
		std::string content;
		FileString(void);
		bool _success;

	public:
		FileString(const char * file_name);
		FileString(FileString & src);
		FileString & operator= (FileString & rhs);
		~FileString(void);
		char * getFileName();
		std::string getContent();
		bool fail();
		bool success();
};

std::ostream & operator<< (std::ostream & o, FileString const & i);

#endif
