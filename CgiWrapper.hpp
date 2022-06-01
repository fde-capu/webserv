/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiWrapper.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/26 14:08:00 by fde-capu          #+#    #+#             */
/*   Updated: 2022/06/01 16:39:23 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIWRAPPER_HPP
# define CGIWRAPPER_HPP

# include "header.hpp"
# include "WebServ.hpp"

struct ws_cgi_reply : ws_reply_instance
{
	ws_cgi_reply(std::string&);
};

class CgiWrapper
{
	private:
		std::string executable;
		int port;
		int listen_sock;
		std::vector<struct pollfd> poll_list;

		CgiWrapper();
		CgiWrapper& operator= (CgiWrapper const &);
		CgiWrapper(CgiWrapper const &);
		void remove_from_poll(int);

	public:
		CgiWrapper(std::string, int);
		~CgiWrapper();
};

std::ostream & operator<< (std::ostream &, CgiWrapper const &);

#endif
