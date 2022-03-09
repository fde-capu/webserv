/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DataFold.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/06 18:40:12 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/09 14:25:42 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DATAFOLD_HPP
# define DATAFOLD_HPP

# include <iostream>
# include <vector>
# include <utility>
# include "StringTools.hpp"
# include <cstdlib>

# define DF_TYPE_NUMBER 2
# define DF_TYPE_STRING	4
# define DF_TYPE_ARRAY	8 // not implemented
# define DF_TYPE_SUB	16
# define DF_QUOTE_SET "\"\'`"
# define DF_KEY_QUOTE "\'"
# define DF_NUM_QUOTE ""
# define DF_VAL_QUOTE "\'"
# define DF_ARRAY_INIT "[ "
# define DF_COMMA " , "
# define DF_ARRAY_END " ]"
# define DF_KVDIV " : "
# define DF_OBJ_INIT "{ "
# define DF_OBJ_END " }"
# define DF_ENDVAL_SET ";"

# define DFK "'" + key + "'" +
# define DF_ERR_NO_KEY DFK " not found."
# define DF_ERR_IS_ARRAY DFK " is array."
# define DF_ERR_NOT_NUMBER DFK " is not a number."

typedef struct datafold_type
{
	int							index;
	int							type;
	std::string					key;
	std::string					val;
	std::vector<datafold_type>	sub;

	operator std::string();
	operator int();
} datafold_t;

class datavec : public std::vector<datafold_t>
{
	public:
		operator std::string();
};

class DataFold : public StringTools
{
	private:
		datavec	core;
		int		index;
	public:
		DataFold(std::string df_data);
		DataFold(void);
		DataFold(DataFold const & src);
		DataFold & operator= (DataFold const & rhs);
		~DataFold(void);
		void push_back(std::string key, std::string val);
		void push_back(std::string key, DataFold sub);
		size_t size() const;
		const datafold_type operator[] (size_t idx) const;
		const std::string operator[] (std::string) const;
		datavec getCore() const;
		int getIndex() const;
		DataFold parse_data(const std::string dst);
		int df_type(std::string);
		const std::string getFirstByKey(std::string key) const;
		std::string eqvals_to_arrstr(std::string key) const;
		std::string quoted_val(datafold_t) const;
		const std::string getValStr(std::string key) const;
		int key_count(std::string key) const;
		void key_count_exists_check(std::string key);
		void key_count_single_check(std::string key);
		operator datavec();
		operator std::string();
		datafold_t get_datafold(std::string key);
		std::vector<int> get_vector_int(std::string key);
		std::vector<std::string> get_vector_str(std::string key);

		template <typename T>
		T get(std::string key)
		{ return get_datafold(key); }
};

std::ostream & operator<< (std::ostream & o, std::vector<datafold_t> const &);
std::ostream & operator<< (std::ostream & o, datafold_type const &);
std::ostream & operator<< (std::ostream & o, DataFold const &);

#endif
