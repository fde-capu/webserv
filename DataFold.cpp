/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DataFold.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/06 18:45:14 by fde-capu          #+#    #+#             */
/*   Updated: 2022/08/03 16:41:43 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DataFold.hpp"

DataFold::operator int() const
{
	if ((core.empty())
	|| (!(core.type & DF_TYPE_NUMBER)))
		return 0;
	return std::atoi(core[0].val.c_str());
}

DataFold DataFold::filter(std::string key) const
{
	DataFold out;
	DataFold dup(*this);
	while (dup.loop())
	{
		if (key == dup.key)
		{
			out.push_back(key, dup.val);
		}
	}
	return out;
}

void DataFold::set(const char* key, const char* val)
{
	return set(std::string(key), std::string(val));
}

void DataFold::set(std::string key, std::string u_val)
{
	this->push_back(key, u_val);
}

DataFold DataFold::get_val() const
{
	DataFold out;
	DataFold dup(*this);
	while (dup.loop())
		out.push_back("", dup.val);
	return out;
}

DataFold DataFold::only_val() const
{
	DataFold dup(this->val);
	dup.set_only_val();
	return dup;
}

void DataFold::set_only_val()
{
	core.type += core.type & DF_TYPE_ONLY_VAL ? 0 : DF_TYPE_ONLY_VAL;
}

DataFold DataFold::get_val(std::string key) const
{
	DataFold out;
	core_check();
	for (size_t i = 0; i < index; i++)
	{
		if (key == core[i].key)
		{
			if (core[i].type & DF_TYPE_SUB)
				out.push_back(core[i].sub);
			else if (core[i].type & DF_TYPE_ARRAY)
			{
				out = splitOutsideQuotes(core[i].val);
			}
			else
				out = core[i].val;
		}
	}
	return out;
}

DataFold DataFold::get_val(std::string key, std::string sub) const
{
	DataFold df_sub;

	core_check();
	key_count_single_check(key);
	for (size_t i = 0; i < index; i++)
		if (key == core[i].key)
		{
			df_sub = get_val(key);
			return df_sub.get_val(sub);
		}
	return DataFold();
}

datafold_t DataFold::get_datafold() const
{
	core_check();
	key_count_single_check(key);
	for (size_t i = 0; i < index; i++)
		if (key == core[i].key)
			return core[i];
	return datafold_t();
}

datafold_t DataFold::get_datafold(std::string key, std::string ksub) const
{
	core_check();
	key_count_single_check(key);
	for (size_t i = 0; i < index; i++)
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
	for (size_t i = 0; i < index; i++)
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
	for (size_t i = 0; i < index; i++)
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

void DataFold::string_check(const datafold_t &df) const
{
	if (!(df.type & DF_TYPE_STRING))
		throw std::invalid_argument(DF_ERR_NOT_STRING);
}

void DataFold::array_check(const datafold_t &df) const
{
	if (!(df.type & DF_TYPE_ARRAY))
		throw std::invalid_argument(DF_ERR_NOT_ARRAY);
}

void DataFold::not_sub_check(const datafold_t &df) const
{
	if (df.type & DF_TYPE_SUB)
		throw std::invalid_argument(DF_ERR_IS_OBJECT);
}

std::vector<std::string> DataFold::get_vector_str() const
{
	std::vector<std::string> out;
	for (size_t i = 0; i < index; i++)
	{
		out.push_back(core[i].val);
	}
	return out;
}

std::vector<std::string> DataFold::get_vector_str(std::string key) const
{
	std::vector<std::string> out;
	core_check();

	for (size_t i = 0; i < index; i++)
		if (core[i].key == key)
		{
			not_sub_check(core[i]);
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
	std::vector<std::string> out;
	core_check();

	for (size_t i = 0; i < index; i++)
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
{ return getCore(); }

DataFold::operator std::vector<std::string>() const
{ return get_vector_str(); }

DataFold::DataFold()
: index(0), loop_index(0), key(""), val(""), type(0)
{ }

DataFold::DataFold(std::string df_data)
: index(0), loop_index(0), key(""), val(""), type(0)
{
	*this = parse_data(df_data);
}

DataFold::DataFold(datafold_t df)
: index(0), loop_index(0), key(""), val(""), type(0)
{
	*this = parse_only_val(df);
}

DataFold::DataFold(const std::vector<std::string> &vs)
: index(0), loop_index(0), key(""), val(""), type(0)
{
	for (size_t i = 0; i < vs.size(); i++)
		push_back("", vs[i]);
}

DataFold& DataFold::operator= (const datafold_t &df)
{
	DataFold out = DataFold(static_cast<std::string>(df));
	core = out.getCore();
	index = out.getIndex();
	loop_index = 0;
	key = "";
	val = "";
	type = 0;
	return *this;
}

DataFold::DataFold(DataFold const & src)
: index(0), loop_index(0), key(""), val(""), type(0)
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

DataFold& DataFold::operator= (DataFold const & rhs)
{
	if (this != &rhs)
	{
		core = rhs.getCore();
		index = rhs.getIndex();
		loop_index = 0;
	}
	return *this;
}

std::string DataFold::quoted_val(const datafold_t &dt) const
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
	for (size_t i = 0; i < index; i++)
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
{ }

size_t DataFold::size() const
{ return core.size(); }

const datafold_type DataFold::operator[] (size_t idx) const
{ return core[idx]; }

void DataFold::core_check() const
{
	if (core.empty())
		throw std::runtime_error(DF_ERR_NO_FILE);
}

bool DataFold::has_key(std::string key) const
{ return static_cast<bool>(key_count(key)); }

int DataFold::key_count(std::string key) const
{
	int out = 0;
	for (size_t i = 0; i < index; i++)
		out += core[i].key == key ? 1 : 0;
	return out;
}

std::string DataFold::get(std::string key) const
{ return get<std::string>(key); }

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
		throw std::invalid_argument(DF_ERR_IS_ARRAY DF_ERR_NOT_SINGLE_KEY);
}

std::string DataFold::get_val_str() const
{ return *this; }

const std::string DataFold::getValStr(std::string key) const
{
	for (size_t i = 0; i < index; i++)
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
		return DF_TYPE_SUB;
	}
	int out = 0;
	if (StringTools::find_outside_quotes(val, " ") != std::string::npos)
	{
		out += DF_TYPE_ARRAY;
		out += isAllNumber(splitOutsideQuotes(val)) ? DF_TYPE_NUMBER : DF_TYPE_STRING;
	}
	else
		out += isNumber(val) ? DF_TYPE_NUMBER : DF_TYPE_STRING;
	return out;
}

std::string DataFold::string() const
{
	return *this;
}

void DataFold::push_back(const datavec &dv)
{ push_back(const_cast<datavec&>(dv)); }

void DataFold::push_back(datavec &dv)
{
	datafold_t entry;
	while (dv.loop())
	{
		entry.index = index++;
		entry.type = dv.type;
		entry.key = dv.key;
		if (!(dv.type & DF_TYPE_SUB))
			entry.val = dv.val;
		else
			entry.sub = DataFold(dv.val);
		core.push_back(entry);
	}
}

DataFold& DataFold::pop()
{
	core.erase(core.begin());
	for (size_t i = 0; i < core.size(); i++)
		core[i].index--;
	index--;
	return *this;
}

void DataFold::push_back(std::string key, std::string val)
{
	hard_trim(key);
	hard_trim(val);
	datafold_t entry;
	entry.index = index++;
	entry.type = df_type(val);
	entry.key = key;
	entry.key = remove_quotes(key);
	if (entry.type & DF_TYPE_ARRAY)
	{
		vstr spl = splitOutsideQuotes(val);
		entry.val = "";
		for (size_t i = 0; i < spl.size(); i++)
		{
			if (StringTools::find_outside_quotes(spl[i], " ") == nopos)
				entry.val += spl[i];
			else
				entry.val += "\"" + escape_char(spl[i], "\"") + "\"";
			if (i + 1 < spl.size())
				entry.val += " ";
		}
	}
	else
		entry.val = remove_quotes(val);
	core.push_back(entry);
}

void DataFold::push_back(datafold_t &df)
{
	df.index = index++;
	core.push_back(df);
}

void DataFold::push_back(const datafold_t &df)
{ push_back(const_cast<datafold_t&>(df)); }

void DataFold::push_back(std::string key, const DataFold &sub)
{
	datafold_t entry;
	entry.index = index++;
	entry.type = DF_TYPE_SUB;
	entry.key = remove_quotes(key);
	entry.sub = sub;
	core.push_back(entry);
}

void DataFold::array_into_inline(std::string& dst) const
{
	size_t p_op;
	size_t p_cl;
	std::string blk;

	p_op = StringTools::find_outside_quotes(dst, "[");
	p_cl = StringTools::find_outside_quotes(dst, "]");
	while (p_op != std::string::npos && p_cl != std::string::npos)
	{
		blk = dst.substr(p_op + 1, p_cl - p_op - 1);
		soft_trim(blk);
		hard_trim(blk);
		substitute_all(blk, ",", " ");
		hard_trim(blk);
		soft_trim(blk);
		if (p_op > 0 && dst.substr(p_op - 1, 1) == ":")
			p_op--;
		dst = dst.substr(0, p_op) + " " + blk + dst.substr(p_cl + 1);
		p_op = StringTools::find_outside_quotes(dst, "[");
		p_cl = StringTools::find_outside_quotes(dst, "]");
	}
}

bool DataFold::empty() const
{ return core.empty(); }

std::string DataFold::clean_before_parse(std::string& dst) const
{
	remove_comments(dst);
	substitute_super(dst, "\n{\n", "{");
	substitute_super(dst, "\n}\n", "}");
	substitute_super(dst, "\n", ";");
	substitute_super(dst, "\t", " ");
	erase_boundaries(dst, ";");
	hard_trim(dst, ";");
	substitute_super(dst, "{;", "{");
	substitute_super(dst, "};", "}");
	substitute_super(dst, ";{", "{");

	soft_trim(dst);
	erase_boundaries(dst, ":,{}[]");
	array_into_inline(dst);
	substitute_super(dst, ":{", "{");
	dual_trim(dst, "{}");
	soft_trim(dst);

	return dst;
}

DataFold DataFold::parse_only_val(const datafold_t &df)
{
	if (df.type & DF_TYPE_SUB)
	{
		return DataFold(df.sub);
	}
	else
	{
		DataFold out;
		datafold_t entry;

		entry.val = df.val;
		hard_trim(entry.val);
		entry.type = df.type + DF_TYPE_ONLY_VAL;
//		entry.key = df.val;
		entry.key = "";
		out.push_back(entry);
		return out;
	}
}

DF DF::parse_data(const str_t jstr)
{
	DF out;
	str_t ops = jstr;
	size_t div_p;
	bool pass;
	str_t key;
	str_t val;
	size_t p[4];

	clean_before_parse(ops);
	verbose(4) << nl << "Parsing: " << ops << nl;

	pass = false;
	while (!pass)
	{
		pass = true;
		p[0] = StringTools::find_outside_quotes_set(ops, " :,");
		p[1] = StringTools::find_outside_quotes_set(ops, "{");
		p[2] = StringTools::find_outside_quotes_set(ops, ";");
		p[3] = StringTools::find_outside_quotes_set(ops, "\n");

		verbose(5)	<< p[0] << ", " << p[1] << ", " \
					<< p[2] << ", " << p[3] << nl;

//		word value && not single word && not subgroup
		if (p[0] < p[1] && p[0] != nopos && p[1] != p[0] + 1)
		{
			pass = false;
			key = ops.substr(0, p[0]);
			ops = ops.substr(p[0] + 1);
			if (p[2] < p[3])
				div_p = StringTools::find_outside_quotes_set(ops, ";");
			else
				div_p = StringTools::find_outside_quotes(ops, "\n");
			val = ops.substr(0, div_p);
			ops = ops.substr(div_p + 1);
			out.push_back(key, val);
			continue ;
		}

//		subgroup && not single word
		if (p[1] < p[0] && p[1] != nopos)
		{
			pass = false;
			key = ops.substr(0, p[1]);
			ops = ops.substr(p[1] + 1);
			div_p = find_closing_bracket(ops);
			val = ops.substr(0, div_p);
			ops = ops.substr(div_p + 1);
			out.push_back(key, parse_data(val));
			continue ;
		}

//		has ; && does not have divisor && is not subgroup && does not break line	
		if (p[2] != nopos && p[0] == nopos && p[1] == nopos && p[3] == nopos)
		{
			pass = false;
			key = ops.substr(0, p[2]);
			val = "";
			ops = ops.substr(p[2] + 1);
			out.push_back(key, val);
			continue;
		}

	}

	verbose(4) << "Parsed: " << static_cast<std::string>(out) << nl << nl;
	return out;
}

bool DataFold::is_single_array() const
{
	return		core.size() == 1 
			&&	core[0].type & DF_TYPE_ARRAY
			;
}

bool DataFold::loop()
{
	if (loop_ended())
	{
		loop_reset();
		return false;
	}

	if (is_single_array())
	{
		key = core[0].key;
		val = nth_word(core[0].val, loop_index + 1);
		type = core[0].type;
		loop_index++;
		return true;
	}

	if (core[loop_index].val == "" && core[loop_index].type & DF_TYPE_SUB)
		val = static_cast<std::string>(core[loop_index].sub);
	else
		val = core[loop_index].val;

	key = core[loop_index].key;
	type = core[loop_index].type;

	loop_index++;
	return true;
}

void DataFold::loop_reset()
{
	loop_index = 0;
	type = 0;
	val = "";
	key = "";
}

bool DataFold::loop_ended()
{
	return is_single_array() ? loop_index >= word_count(core[0].val)
							 : loop_index >= index;
}

bool DataFold::not_ended()
{ return !loop_ended(); }

std::ostream& operator<< (std::ostream& o, DataFold const& self)
{
	o << self.string();
	return o;
}

std::ostream& operator<< (std::ostream& o, datavec const& self)
{
	o << self.string();
	return o;
}
