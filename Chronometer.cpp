/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Chronometer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/08 11:59:38 by fde-capu          #+#    #+#             */
/*   Updated: 2022/07/27 15:56:38 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Chronometer.hpp"
#include <ctime>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>

Chronometer::Chronometer(void)
: _start(0), _mark(0)
{ btn_reset(); }

Chronometer::operator double()
{
	btn_stop();
	return read_last();
}

std::ostream& operator<< (std::ostream& o, Chronometer& self)
{
	self.btn_stop();
	o << "::: " << std::fixed << std::showpoint \
		<< std::setprecision(CHRONOMETER_PRECISION);
	o << self.read_last();
	o << " :::";
	return o;
}

Chronometer::~Chronometer(void)
{ return ; }

double Chronometer::read_last() const
{ return _mark; }

void Chronometer::btn_reset()
{ this->_start = std::clock(); }

void Chronometer::btn_stop()
{
	clock_t now = std::clock();
	this->_mark = static_cast<double>(now - _start) \
		/ CLOCKS_PER_SEC * 1000;
}
