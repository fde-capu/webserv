/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ArgVal.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 16:23:02 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/10 16:44:04 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ARGVAL_HPP
# define ARGVAL_HPP

# include <iostream>
# include "FileString.hpp"

class ArgVal
{
	private:
		FileString board;
		ArgVal();
	public:
		ArgVal(const char * file);
		ArgVal(ArgVal const & src);
		ArgVal & operator= (ArgVal const & rhs);
		~ArgVal();
		FileString& getBoard()const;
};

std::ostream & operator<< (std::ostream & o, ArgVal const & i);

#endif
