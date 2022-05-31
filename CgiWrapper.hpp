/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiWrapper.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/26 14:08:00 by fde-capu          #+#    #+#             */
/*   Updated: 2022/05/31 12:38:10 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIWRAPPER_HPP
# define CGIWRAPPER_HPP

# include <iostream>
# include "WebServ.hpp"
# include "bladefs.hpp"

class CgiWrapper
{
	private:
		std::string executable;
		int port;
		int listen_sock;
		std::vector<struct pollfd> poll_list;
//		std::map<int, ws_server_instance*> fd_to_instance;

		CgiWrapper();
		CgiWrapper& operator= (CgiWrapper const &);
		CgiWrapper(CgiWrapper const &);
		void remove_from_poll(int);

	public:
		CgiWrapper(std::string, int);
		~CgiWrapper();
		bool fail;
};

std::ostream & operator<< (std::ostream &, CgiWrapper const &);

#endif
