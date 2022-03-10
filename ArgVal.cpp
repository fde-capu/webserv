/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ArgVal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 16:23:55 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/10 20:08:03 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ArgVal.hpp"

ArgVal::ArgVal(int argc, char ** argv)
{
	_board.load(AV_FILE_BOARD);

	try {
		if (argc != _board.get<int>("argc", "fixed"))
			_fail = true;
	} catch(std::exception&){}

	try {
		if (argc > _board.get<int>("argc", "max"))
			_fail = true;
	} catch(std::exception&){}

	try {
		if (argc < _board.get<int>("argc", "min"))
			_fail = true;
	} catch(std::exception&){}

	int i = 1;
	std::vector<std::string> value_test = _board.get<std::vector<std::string> >("argv", itoa(i));
	std::cout << "]]]]" << value_test[0] << std::endl;
	std::cout << "]]]]" << value_test[1] << std::endl;
	std::cout << "]]]]" << value_test[2] << std::endl;
	std::cout << "]]]]" << value_test[3] << std::endl;
	(void)argv;
}

bool ArgVal::fail() const
{ return _fail; }

ArgVal::ArgVal()
{
	_board.load(AV_FILE_BOARD);
//	_board = FileString(AV_FILE_BOARD);
}

ArgVal::ArgVal(ArgVal const & src)
{
	*this = src;
	return ;
}

ArgVal & ArgVal::operator= (ArgVal const & rhs)
{
	if (this != &rhs)
	{
		_board = rhs.getBoard();
		_config = rhs.getConfig();
	}
	return *this;
}

FileString& ArgVal::getConfig() const
{ return *const_cast<FileString*>(&_config); }

FileString& ArgVal::getBoard() const
{ return *const_cast<FileString*>(&_board); }

std::ostream & operator<< (std::ostream & o, ArgVal const & self)
{
	o << "::ArgVal::" << std::endl;
	o << self.getBoard() << std::endl;
	o << self.getConfig() << std::endl;
	return o;
}

ArgVal::~ArgVal()
{
	return ;
}
