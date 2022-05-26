/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiWrapper.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/26 14:08:00 by fde-capu          #+#    #+#             */
/*   Updated: 2022/05/26 16:29:07 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIWRAPPER_HPP
# define CGIWRAPPER_HPP

# include <iostream>
# include "bladefs.hpp"

class CgiWrapper
{
	private:
		std::string executable;
		size_t port;
		CgiWrapper();
		CgiWrapper& operator= (CgiWrapper const &);
		CgiWrapper(CgiWrapper const &);
	public:
		CgiWrapper(std::string, size_t);
		~CgiWrapper();
};

std::ostream & operator<< (std::ostream &, CgiWrapper const &);

#endif
