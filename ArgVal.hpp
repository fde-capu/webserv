/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ArgVal.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 16:23:02 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/10 18:05:50 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ARGVAL_HPP
# define ARGVAL_HPP

# include <iostream>
# include "FileString.hpp"

# define AV_FILE_BOARD ""
//# define AV_FILE_CONFIG ""

class ArgVal : protected FileString
{
	private:
		FileString _board;
		FileString _config;
	public:
		ArgVal();
		ArgVal(ArgVal const & src);
		ArgVal & operator= (ArgVal const & rhs);
		~ArgVal();
		FileString& getBoard()const;
		FileString& getConfig()const;
};

std::ostream & operator<< (std::ostream & o, ArgVal const & i);

#endif
