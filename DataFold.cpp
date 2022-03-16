/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DataFold.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/06 18:45:14 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/16 19:36:06 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DataFold.hpp"

datafold_t DataFold::get_datafold(std::string key, std::string ksub) const
{
	core_check();
	key_count_single_check(key);
	for (int i = 0; i < index; i++)
		if (key == core[i].key)
			for (size_t j = 0; j < core[i].sub.size(); j++)
				if (ksub == core[i].sub[j].key)
					return core[i].sub[j];
	return datafold_t();
}

std::vector<int> DataFold::get_vector_int(std::string key) const
{
	core_check();
	std::vector<int> out;
	for (int i = 0; i < index; i++)
		if (core[i].key == key)
		{
			if (!(core[i].type & DF_TYPE_NUMBER))
				throw std::invalid_argument(DF_ERR_NOT_NUMBER);
			if (!(core[i].type & DF_TYPE_ARRAY))
				out.push_back(std::atoi(core[i].val.c_str()));
			else
			{
				std::vector<std::string> arr_split = splitOutsideQuotes(core[i].val);
				for (size_t j = 0; j < arr_split.size(); j++)
					out.push_back(std::atoi(arr_split[j].c_str()));
			}
		}
	return out;
}

std::vector<int> DataFold::get_vector_int(std::string key, std::string ksub) const
{
	core_check();
	std::vector<int> out;
	for (int i = 0; i < index; i++)
		if (key == core[i].key)
			for (size_t j = 0; j < core[i].sub.size(); j++)
				if (ksub == core[i].sub[j].key)
				{
					if (!(core[i].sub[j].type & DF_TYPE_NUMBER))
						throw std::invalid_argument(DF_ERR_NOT_NUMBER);
					if (!(core[i].sub[j].type & DF_TYPE_ARRAY))
						out.push_back(std::atoi(core[i].sub[j].val.c_str()));
					else
					{
						std::vector<std::string> arr_split = splitOutsideQuotes(core[i].sub[j].val);
						for (size_t j = 0; j < arr_split.size(); j++)
							out.push_back(std::atoi(arr_split[j].c_str()));
					}
				}
	return out;
}

std::vector<std::string> DataFold::get_vector_str(std::string key) const
{
	core_check();
	std::vector<std::string> out;
	for (int i = 0; i < index; i++)
		if (core[i].key == key)
		{
			if (core[i].type & DF_TYPE_SUB)
				throw std::invalid_argument(DF_ERR_IS_OBJECT);
			if (!(core[i].type & DF_TYPE_ARRAY))
				out.push_back(core[i].val);
			else
			{
				std::vector<std::string> arr_split = splitOutsideQuotes(core[i].val);
				for (size_t j = 0; j < arr_split.size(); j++)
					out.push_back(arr_split[j]);
			}
		}
	return out;
}

std::vector<std::string> DataFold::get_vector_str(std::string key, std::string ksub) const
{
	core_check();
	std::vector<std::string> out;
	for (int i = 0; i < index; i++)
		if (key == core[i].key)
			for (size_t j = 0; j < core[i].sub.size(); j++)
				if (ksub == core[i].sub[j].key)
				{
					if (core[i].sub[j].type & DF_TYPE_SUB)
						throw std::invalid_argument(DF_ERR_IS_OBJECT);
					if (!(core[i].sub[j].type & DF_TYPE_ARRAY))
						out.push_back(core[i].sub[j].val);
					else
					{
						std::vector<std::string> arr_split = splitOutsideQuotes(core[i].sub[j].val);
						for (size_t j = 0; j < arr_split.size(); j++)
							out.push_back(arr_split[j]);
					}
				}
	return out;
}

DataFold::operator std::string() const
{
	std::stringstream o;
	o << getCore();
	return o.str();
}

std::ostream & operator<< (std::ostream & o, DataFold const & self)
{
	o << self.getCore();
	return o;
}

DataFold::DataFold()
: index(0), loop_index(0)
{
	return ;
}

DataFold::DataFold(std::string df_data)
{
	*this = parse_data(df_data);
}

DataFold::DataFold(DataFold const & src)
: StringTools()
{
	*this = src;
	return ;
}

datavec DataFold::getCore() const
{ return core; }

DataFold::operator datavec() const
{ return core; }

int DataFold::getIndex() const
{ return index; }

DataFold & DataFold::operator= (DataFold const & rhs)
{
	if (this != &rhs)
	{
		core = rhs.getCore();
		index = rhs.getIndex();
		loop_index = 0;
	}
	return *this;
}

std::string DataFold::quoted_val(datafold_t dt) const
{
	std::stringstream out;
	if (dt.type & DF_TYPE_NUMBER)
		out << apply_quotes(dt.val, DF_NUM_QUOTE);
	if (dt.type & DF_TYPE_STRING)
		out << apply_quotes(dt.val, DF_VAL_QUOTE);
	if (dt.type & DF_TYPE_SUB)
		out << dt.sub;
	return out.str();
}

std::string DataFold::eqvals_to_arrstr(std::string key) const
{
	bool put_comma = false;
	std::string out(DF_ARRAY_INIT);
	for (int i = 0; i < index; i++)
	{
		if (core[i].key == key)
		{
			if (put_comma)
				out += DF_COMMA;
			out += quoted_val(core[i]);
			put_comma = true;
		}
	}
	out += DF_ARRAY_END;
	return out;
}

DataFold::~DataFold()
{ return ; }

size_t DataFold::size() const
{ return core.size(); }

const datafold_type DataFold::operator[] (size_t idx) const
{ return core[idx]; }

void DataFold::core_check() const
{
	if (core.empty())
		throw std::runtime_error(DF_ERR_NO_FILE);
}

int DataFold::key_count(std::string key) const
{
	int out = 0;
	for (int i = 0; i < index; i++)
		out += core[i].key == key ? 1 : 0;
	return out;
}

int DataFold::key_count_exists_check(std::string key) const
{
	int out = key_count(key);
	if (!out)
		throw std::invalid_argument(DF_ERR_NO_KEY);
	return out;
}

void DataFold::key_count_single_check(std::string key) const
{
	int count = key_count_exists_check(key);
	if (count != 1)
		throw std::invalid_argument(DF_ERR_IS_ARRAY);
}

const std::string DataFold::getValStr(std::string key) const
{
	for (int i = 0; i < index; i++)
		if (key == core[i].key)
			return core[i].val;
	return std::string("");
}

const std::string DataFold::operator[] (std::string key) const
{
	int kc = key_count(key);

	if (!kc)
		return std::string("");
	if (kc == 1)
		return getValStr(key);
	return eqvals_to_arrstr(key);
}

int DataFold::df_type(std::string val)
{
	hard_trim(val);
	if (*val.begin() == '{' && *(val.end() - 1) == '}')
	{
		std::cout << DF_TYPE_SUB << std::endl;
		return DF_TYPE_SUB;
	}
	int out = 0;
	if (find_outside_quotes(val, " ") != std::string::npos)
	{
		out += DF_TYPE_ARRAY;
		out += isAllNumber(splitOutsideQuotes(val)) ? DF_TYPE_NUMBER : DF_TYPE_STRING;
	}
	else
		out += isNumber(val) ? DF_TYPE_NUMBER : DF_TYPE_STRING;
	return out;
}

void DataFold::push_back(std::string key, std::string val)
{
	datafold_t entry;
	entry.index = index++;
	entry.type = df_type(val);
	entry.key = correct_quotes(key);
	if (entry.type & DF_TYPE_ARRAY)
	{
		std::vector<std::string> spl = splitOutsideQuotes(val);
		entry.val = "";
		for (size_t i = 0; i < spl.size(); i++)
		{
			entry.val += correct_quotes(spl[i]);
			if (i + 1 < spl.size())
				entry.val += " ";
		}
	}
	else
	{
		entry.val = correct_quotes(val);
	}
	core.push_back(entry);
}

void DataFold::push_back(std::string key, DataFold sub)
{
	datafold_t entry;
	entry.index = index++;
	entry.type = DF_TYPE_SUB;
	entry.key = correct_quotes(key);
	entry.sub = sub;
	core.push_back(entry);
}

void DataFold::array_into_inline(std::string& dst) const
{
	size_t p_op;
	size_t p_cl;
	std::string blk;

	p_op = find_outside_quotes(dst, "[");
	p_cl = find_outside_quotes(dst, "]");
	while (p_op != std::string::npos && p_cl != std::string::npos)
	{
		blk = dst.substr(p_op + 1, p_cl - p_op - 1);
		soft_trim(blk);
		hard_trim(blk);
		substitute_all(blk, ",", " ");
		hard_trim(blk);
		soft_trim(blk);
		if (dst.substr(p_op - 1, 1) == ":")
			p_op--;
		dst = dst.substr(0, p_op) + " " + blk + dst.substr(p_cl + 1);
		p_op = find_outside_quotes(dst, "[");
		p_cl = find_outside_quotes(dst, "]");
	}
}

std::string DataFold::clean_before_parse(std::string& dst) const
{
	remove_comments(dst);
	substitute_super(dst, "\n{\n", "{");
	substitute_super(dst, "\n}\n", "}");
	substitute_super(dst, "\n", ";");
	remove_all(dst, "\t");
	erase_boundaries(dst, ";");
	hard_trim(dst, ";");
	substitute_super(dst, "{;", "{");
	substitute_super(dst, "};", "}");
	substitute_super(dst, ";{", "{");

	dual_trim(dst, "{}");
	soft_trim(dst);
	erase_boundaries(dst, ":,{}[]");
	array_into_inline(dst);
	substitute_super(dst, ":{", "{");
	return dst;
}

DataFold DataFold::parse_data(const std::string str)
{
	DataFold out;
	std::string ops = str;
	size_t div_pos;
	bool pass;
	std::string key;
	std::string val;
	size_t pos[4];

	clean_before_parse(ops);

	if (VERBOSE >= 2)
		std::cout << std::endl << "Parsing: " << ops << std::endl;

	pass = false;
	while (!pass)
	{
		pass = true;
		pos[0] = find_outside_quotes_set(ops, " :");
		pos[1] = find_outside_quotes_set(ops, "{");
		pos[2] = find_outside_quotes_set(ops, ";,");
		pos[3] = find_outside_quotes_set(ops, "\n");

		if (VERBOSE >= 3)
			std::cout << pos[0] << ", " << pos[1] << ", " << pos[2] << ", " << pos[3] << std::endl;

		if (pos[0] < pos[1] && pos[0] != std::string::npos && pos[1] != pos[0] + 1)
		{
			pass = false;
			key = ops.substr(0, pos[0]);
			ops = ops.substr(pos[0] + 1);
			if (pos[2] < pos[3])
				div_pos = find_outside_quotes_set(ops, ";,");
			else
				div_pos = find_outside_quotes(ops, "\n");
			val = ops.substr(0, div_pos);
			ops = ops.substr(div_pos + 1);
			hard_trim(key);
			hard_trim(val);
			out.push_back(key, val);
			continue ;
		}
		if (pos[1] < pos[0] && pos[1] != std::string::npos)
		{
			pass = false;
			key = ops.substr(0, pos[1]);
			ops = ops.substr(pos[1] + 1);
			div_pos = find_closing_bracket(ops);
			val = ops.substr(0, div_pos);
			ops = ops.substr(div_pos + 1);
			hard_trim(key);
			hard_trim(val);
			out.push_back(key, parse_data(val));
			continue ;
		}
	}

	if (VERBOSE >= 2)
		std::cout << "Parsed: " << out << std::endl << std::endl;

	return out;
}

bool DataFold::loop()
{
	if (loop_index == index)
	{
		loop_index = 0;
		return false;
	}
	key = core[loop_index].key;
	if (core[loop_index].type & DF_TYPE_SUB)
		val = core[loop_index].sub;
	else
		val = core[loop_index].val;
	loop_index++;
	return true;
}
