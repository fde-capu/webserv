/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ArgVal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 16:23:55 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/23 20:36:26 by fde-capu         ###   ########.fr       */
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
	int bval = _board.get_val("argc", "fixed");
	if (bval && bval != argc)
	{
		verbose(1) << "argc does not match.";
		_fail = true;
	}

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

bool ArgVal::comply_argval_params(DataFold board, DataFold config)
{
	DataFold par;

	while (board.loop())
	{
		verbose(2) << " > " << board.key << " :=: " << board.val << std::endl;
		if (board.key == "accept_unique")
		{
			par = board.get<DataFold>("accept_unique");
			while (par.loop())
			{
				verbose(3) << "   > " << par.val << std::endl;
				if (config.key_count(par.val) > 1)
				{
					verbose(1) << par.val << " is not unique." << std::endl;
					return false;
				}
			}
		}
		if (board.key == "mandatory")
		{
			par = board.get<DataFold>("mandatory");
			while (par.loop())
			{
				verbose(3) << "   > " << par.val << std::endl;
				if (!(config.key_count(par.val)))
				{
					verbose(1) << par.val << " is not present." << std::endl;
					return false;
				}
			}
		}
		if (board.type & DF_TYPE_SUB)
			return comply_argval_params(board.get_val(board.key), config.get<DataFold>(board.key));
	}

	return true;
}

bool ArgVal::comply()
{
	verbose(1) << "Comply:" << std::endl;
	DataFold comply(_board.get_val("comply"));
	DataFold par;
	DataFold con;
	bool valid;
	if (!comply_argval_params(comply, _config))
		return false;
	while (_config.loop())
	{
//		verbose(3) << "Is this allowed? " << _config.key << nl;
		valid = false;
		while (comply.loop())
		{
//			verbose(3) << " -- " << comply.key << ":" << comply.val << nl;
			if ((comply.key == "accept_unique" ||
				comply.key == "accept" ||
				comply.key == "mandatory"
				)&&
				(find_outside_quotes(comply.val, _config.key) != nopos))
			{
				if (!(_config.type & DF_TYPE_SUB))
				{
					valid = true;
					comply.loop_reset();
					break ;
				}
				else
				{
					par = _config.get<DataFold>(_config.key);
					while (par.loop())
					{
//						xx << "(((" << par.key << " >> " << par.val << ")))" << nl;
					}
//					xx << _config.key << "YUKES";
				}
			}
		}
		if (!valid)
		{
			verbose(1) << "- " << _config.key << " not valid." << nl;
			return false;
		}
	}

	std::cout << " General testing:" << std::endl;
	DataFold tp = _config.get<DataFold>("test_port");
//	std::cout << "tp: " << tp << std::endl;
	while (tp.loop())
	{
		std::cout << "\t" << tp.key << "||" << tp.val << std::endl;
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
: _fail(false)
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

//std::ostream & operator<< (std::ostream & o, ArgVal const & self)
//{
//	o << "::ArgVal::" << std::endl;
//	o << "_board: " << self.getBoard() << std::endl << "_config: " << self.getConfig();
//	return o;
//}

ArgVal::~ArgVal()
{
	return ;
}
