/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ArgVal.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 16:23:02 by fde-capu          #+#    #+#             */
/*   Updated: 2022/04/20 16:04:31 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ARGVAL_HPP
# define ARGVAL_HPP

# define AGF_NUMBER 1
# define AGF_WORD 2
# define AGF_ONLY 4
# define AGF_FIXED_LEN 8
# define AGF_FILE_NAME 16
# define AGF_BOOL 32
# define AGF_NUMBER_TIL_FILE_NAME 64
# define AGF_NUMBER_THEN_URI 128
# define AGF_WORD_TIL_NUMBER 256
# define AGF_URI 512

# include "FileString.hpp"
//# include <iostream>
//# include "DataFold.hpp"
//# include <stdlib.h> // no itoa in <cstdlib>
//# include <vector>

//# define AV_FILE_BOARD "argval.conf"
//# define AV_FILE_CONFIG ""

class ArgVal
{
	private:
		FileString _board;
		FileString _config;
		bool _fail;
		char * _board_file_name;

	public:
		ArgVal(int, char **, const char *);
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
		FileString& getBoard() const;
		FileString& getConfig() const;
		bool fail() const;
		bool success() const;

		bool comply();
		bool comply_check(DataFold, DataFold);
		bool comply_argval_params(DataFold, DataFold);
		bool comply_config_keys(DataFold, DataFold);
		bool validate_by_board_key(DataFold, DataFold);

		size_t count_keys(DataFold, std::string) const;
};

//std::ostream & operator<< (std::ostream &, ArgVal const &);
#endif
