/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Chronometer.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/08 11:59:23 by fde-capu          #+#    #+#             */
/*   Updated: 2021/12/08 11:59:23 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHRONOMETER_HPP
# define CHRONOMETER_HPP

# define CHRONOMETER_PRECISION 8

# include <iostream>
# include <string>
# include <sstream>
# include <fstream>

class Chronometer
{
	private:
		static const char * chrono_file_name;
	public:
		std::ofstream chrono_file;
		Chronometer(void);
		Chronometer(Chronometer const & src);
		Chronometer &	operator = (Chronometer const & rhs);
		~Chronometer(void);
		std::string start();
		static clock_t _start;
};

std::ostream &	operator << (std::ostream & o, Chronometer const & i);

#endif
