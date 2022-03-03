/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Encapsulator.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/06 02:51:39 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/03 20:33:00 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENCAPSULATOR_HPP
# define ENCAPSULATOR_HPP

# include <cstring>
# include <sstream>
# include <memory>

class Encapsulator
{
	private:
		std::string body;
		std::stringstream response;
		std::string full_body;

	public:
		Encapsulator();
		Encapsulator(const char * u_body);
		Encapsulator(Encapsulator const & src);
		Encapsulator & operator= (Encapsulator const & rhs);
		~Encapsulator();
		const void * encapsulate();
		int full_length();
		std::string getBody() const;
};

#endif
