/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Chronometer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/08 11:59:38 by fde-capu          #+#    #+#             */
/*   Updated: 2021/12/08 11:59:38 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Chronometer.hpp"
#include <ctime>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>

clock_t Chronometer::_start = std::clock();

Chronometer::Chronometer(void)
{
	this->_start = std::clock();
	return ;
}

Chronometer::Chronometer(Chronometer const & src)
{
	*this = src;
	return ;
}

Chronometer & Chronometer::operator = (Chronometer const & rhs)
{
	static_cast<void>(rhs);
	return *this;
}

std::ostream & operator << (std::ostream & o, Chronometer const & self)
{
	clock_t now = std::clock();
	double elapsed = static_cast<double>(now - self._start) / CLOCKS_PER_SEC * 1000;
	o << "::: " << std::fixed << std::showpoint << std::setprecision(CHRONOMETER_PRECISION);
	o << elapsed;
	o << " :::";
	return o;
}

Chronometer::~Chronometer(void)
{
	return ;
}

std::string Chronometer::start()
{
	this->_start = std::clock();
	return "::: clock :::";
}
