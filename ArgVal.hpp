/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ArgVal.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 16:23:02 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/14 21:02:23 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ARGVAL_HPP
# define ARGVAL_HPP

# include <iostream>
# include "FileString.hpp"
# include "StringTools.hpp"
# include <stdlib.h> // no itoa in <cstdlib>
# include <vector>

//# define AV_FILE_BOARD "argval.conf"
//# define AV_FILE_CONFIG ""

class ArgVal : public StringTools
{
	private:
		FileString _board;
		FileString _config;
		bool _fail;
		char * _board_file_name;

	public:
		ArgVal(int, char **, char *);
		ArgVal(int, char **);
		ArgVal();
		ArgVal(ArgVal const & src);
		ArgVal & operator= (ArgVal const &);
		~ArgVal();

		typedef DataFold::vint vint;
		typedef DataFold::vstr vstr;

		int argc;
		char ** argv;
		void load_conditions(char *);
		void run();
		FileString& getBoard()const;
		FileString& getConfig()const;
		bool fail() const;

		bool comply() const;
};

std::ostream & operator<< (std::ostream &, ArgVal const &);

#endif
