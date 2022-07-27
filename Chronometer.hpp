/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Chronometer.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/08 11:59:23 by fde-capu          #+#    #+#             */
/*   Updated: 2022/07/27 15:56:41 by fde-capu         ###   ########.fr       */
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
