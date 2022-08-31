/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Chronometer.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/08 11:59:23 by fde-capu          #+#    #+#             */
/*   Updated: 2022/08/31 14:25:11 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** Instructions:
**
** // Instantiate:
** Chronometer t;
**
** // On every check it carries a new value, ex.:
** while (t < 10) { ... }
**
** // Reset to zero:
** t.btn_reset();
**
** // Mark for reading:
** t.btn_stop();
**
** // Read last stop:
** t.read_last()
**
** // operator<< is implemented, shows current.
*/

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
		clock_t _start;
		double _mark;
	public:
		std::ofstream chrono_file;
		Chronometer(void);
		~Chronometer(void);
		void btn_reset();
		void btn_stop();
		double read_last() const;
		operator double();
	private:
		Chronometer(Chronometer const & src);
		Chronometer& operator= (Chronometer const & rhs);
};

std::ostream& operator<< (std::ostream& o, Chronometer& i);

#endif
