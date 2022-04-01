/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ArgVal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 16:23:55 by fde-capu          #+#    #+#             */
/*   Updated: 2022/04/01 18:42:27 by fde-capu         ###   ########.fr       */
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
			if (!isFileName(std::string(argv[1])))
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
	DataFold con;
	DataFold foo;
	size_t count;

	verbose(3) << "### cap comply_argval_params > " << board.string() << " >> " << config.string() << std::endl;

	while (board.loop())
	{
		verbose(4) << "  cap > " << board.key << " :=: " << board.val << std::endl;

		if (board.type & DF_TYPE_SUB)
		{
			con = config.get_val(board.key);
			verbose(4) << "  sub   > " << board.val << " >> " << con.string() << std::endl;
			if (!comply_argval_params(board.get_val(board.key), con))
				return false;
		}

		if (board.key == "accept_unique")
		{
			par = board.get<DataFold>("accept_unique");
			while (par.loop())
			{
				verbose(4) << "  par   > " << par.val << std::endl;
				count = 0;
				for (size_t i = 0; i < config.size(); i++)
				{
					con = config[i];
					if (par.val == config[i].key && ++count > 1)
					{
						verbose(1) << par.val << " is not unique." << std::endl;
						return false;
					}
				}
			}
		}

		if (board.key == "mandatory")
		{
			par = board.get<DataFold>("mandatory");
			while (par.loop())
			{
				verbose(4) << "  mandatory   > " << par.val << std::endl;
				count = 0;
				while (config.loop())
				{
					if (par.val == config.key)
						count++;
				}
				if (!count)
				{
					verbose(1) << par.val << " is not present." << std::endl;
					return false;
				}
			}
		}

		con = config.get<DataFold>(board.key);
		if (con.empty())
			continue;

		par = board.get<DataFold>(board.key);
		int set_flags = 0;
		size_t fixed_len = 0;

		while (par.loop())
		{
			if (par.val == "number")
				set_flags += AGF_NUMBER;
			if (par.val == "word")
				set_flags += AGF_WORD;
			if (par.val == "only")
				set_flags += AGF_ONLY;
			if (isNumber(par.val))
			{
				set_flags += AGF_FIXED_LEN;
				fixed_len = atoi(par.val.c_str());
			}
			if (par.val == "file_name")
				set_flags += AGF_FILE_NAME;
			if (par.val == "bool")
				set_flags += AGF_BOOL;
			if (par.val == "number*file_name")
				set_flags += AGF_NUMBER_TIL_FILE_NAME;
		}

		while (con.loop())
		{
			verbose(4) << "con_loop " << con.key << " : " << con.val << std::endl;
			if (set_flags & AGF_NUMBER && !isNumber(con.val))
			{
				verbose(1) << con.val << " is not a number." << std::endl;
				return false;
			}
			if (set_flags & AGF_WORD && !isWord(con.val))
			{
				verbose(1) << con.val << " is not valid word." << std::endl;
				return false;
			}
			if (set_flags & AGF_ONLY)
			{
				if (!isWordInWordSet(con.val, par.get_val().pop()))
				{
					verbose(1) << con.val << " invalid." << std::endl;
					return false;
				}
			}
			if (set_flags & AGF_FIXED_LEN)
			{
				foo = con.get_val();
				if ((con.is_single_array() && foo.size() != fixed_len)
					)
				{
					verbose(1) << con.string() << " has incorrect number of parameters." << std::endl;
					return false;
				}
			}
			if (set_flags & AGF_FILE_NAME)
			{
				foo = splitOutsideQuotes(con.val);
				while (foo.loop())
				{
					if (!isFileName(foo.val))
					{
						verbose(1) << foo.val << " is not a valid file name." << std::endl;
						return false;
					}
				}
			}
			if (set_flags & AGF_BOOL)
			{
				if (!isBoolStr(con.val))
				{
					verbose(1) << con.key << " must be a boolean." << std::endl;
					return false;
				}
			}
			if (set_flags & AGF_NUMBER_TIL_FILE_NAME)
			{
				foo = splitOutsideQuotes(con.val);
				while (foo.loop())
				{
					if (!foo.loop_ended())
					{
						if (!isNumber(foo.val))
						{
							verbose(1) << con.key << " " << con.val << " bad syntax." << std::endl;
							return false;
						}
					}
					else
					{
						if (!isFileName(foo.val))
						{
							verbose(1) << foo.val << " is not a valid file name." << std::endl;
							return false;
						}
					}
				}
			}
		}
	}
	if (board.empty() && !config.empty())
	{
		verbose(3) << "> " << config.string() << " is invalid." << std::endl;
		return false;
	}
	verbose(3) << "> " << config.string() << " is valid." << std::endl;
	return true;
}

size_t ArgVal::count_keys(DataFold data, std::string key) const
{
	size_t out = 0;
	while (data.loop())
	{
		if (data.val == key)
			out++;
	}
	return out;
}

bool ArgVal::comply_config_keys(DataFold board, DataFold config)
{
	bool valid;
	DataFold par;

	verbose(3) << "### cck comply_config_keys > " << board.string() << " >> " << config.string() << std::endl;
	while (config.loop())
	{
		verbose(3) << "  config > " << config.key << " :=: " << config.val << " (" << config.type << ")" << nl;
		valid = false;
		while (board.loop())
		{
			verbose(3) << "  board > " << board.key << " :=: " << board.val << nl;
			if (board.type & DF_TYPE_SUB)
			{
				std::cout << "HERE" << std::endl;
				valid = comply_config_keys(board.get_val(board.key), config.val);
			}
//			if (board.type & DF_TYPE_ARRAY)
//			{
//				std::cout << "Array! " << nl;
//			}
			DataFold bv = board.get_datafold();
			size_t count = count_keys(bv, config.key);
			if (board.key == "accept" && count)
				valid = true;
			else if (board.key == "accept_unique" && count == 1)
				valid = true;
			else if (board.key == "mandatory" && !count)
				valid = true;
			else if (count)
				valid = true;
			if (valid)
			{
				board.loop_reset();
				break;
			}
		}
		if (!valid)
		{
			verbose(1) << config.key << " is not valid." << nl;
			return false;
		}
	}
	verbose(3) << "> " << config.string() << " is valid." << std::endl;
	return true;
}

bool ArgVal::comply_check(DataFold board, DataFold config)
{
	return comply_argval_params(board, config) && comply_config_keys(board, config);
}

bool ArgVal::comply()
{
	DataFold comply(_board.get_val("comply"));
	if (!comply_check(comply, _config))
		return false;

	return true;
}

bool ArgVal::fail() const
{ return _fail; }

bool ArgVal::success() const
{ return !_fail; }

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
