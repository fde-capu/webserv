/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ArgVal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 16:23:55 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/16 19:41:08 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ArgVal.hpp"

ArgVal::ArgVal(int argc, char ** argv, char * u_board_file_name)
: _fail(false), _board_file_name(u_board_file_name),
  argc(argc), argv(argv)
{
	_board.load(_board_file_name);
	run();
}

ArgVal::ArgVal(int argc, char ** argv)
: _fail(false), argc(argc), argv(argv)
{}

void ArgVal::run()
{
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

	vstr vt;
	int argi = 0;
	while (++argi < argc)
	{
		vt = _board.get<vstr>("argv", itoa(argi));
		if (vt.size() && vt[0] == "file_name")
		{
			if (!valid_file_name(std::string(argv[1])))
				_fail = true;
			if (vt.size() > 1 && vt[1] == "comply")
			{
				_config.load(argv[argi]);
				if (!comply())
					_fail = true;
			}
		}
	}
}

void ArgVal::load_conditions(char * u_condition_file)
{
	_board_file_name = u_condition_file;
	_board.load(_board_file_name);
	run();
}

ArgVal::ArgVal()
: _fail(false)
{}

bool ArgVal::comply() const
{
	std::cout << " =============== Comply: ==============" << std::endl;
	std::cout << *this << std::endl;
	DataFold comply(_board.get<DataFold>("comply"));
	DataFold par;
	std::cout << comply << std::endl;
	while (comply.loop())
	{
		std::cout << " > " << comply.key << " <1> " << comply.val << std::endl;
		if (comply.key == "accept_unique_keys")
		{
			par = comply.get<DataFold>("accept_unique_keys");
			while (par.loop())
			{
				std::cout << "  > " << par.key << " <2> " << par.val << std::endl;
			}
		}
		std::cout << std::endl;
	}
//	std::string argvt(_board.get<std::string>("single"));
//	std::cout << "0 ----->" << argvt << std::endl;
//	std::cout << "_board: " << _board << std::endl;
//	DataFold cply(_board.get<DataFold>("comply"));
//	std::cout << "1 ----->" << cply << std::endl;
//	std::cout << "2 ----->" << cply.get<std::string>("server") << std::endl;
//	std::string cplystr = _board.get<std::string>("comply");
//	std::cout << "3 ----->" << cplystr << std::endl;
//	std::string cplystr2 = cply;
//	std::cout << "4 ----->" << cplystr2 << std::endl;
	return true;
}

bool ArgVal::fail() const
{ return _fail; }

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
		_fail = rhs.fail();
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
	o << "_board: " << self.getBoard() << std::endl << "_config: " << self.getConfig();
	return o;
}

ArgVal::~ArgVal()
{
	return ;
}
