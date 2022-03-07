/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DataFold.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/06 18:40:12 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/07 17:08:53 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DATAFOLD_HPP
# define DATAFOLD_HPP

# include <iostream>
# include <vector>
# include <utility>
# include "StringTools.hpp"

# define DF_TYPE_NUMBER 2
# define DF_TYPE_STRING	4
# define DF_TYPE_ARRAY	8 // not implemented
# define DF_TYPE_SUB	16
# define DF_QUOTE_SET "\"\'`"
# define DF_KEY_QUOTE '\''
# define DF_NUM_QUOTE ""
# define DF_VAL_QUOTE '\''

typedef std::pair<std::string, std::string> pair_str;

typedef struct datafold_type
{
	int							index;
	int							type;
	std::string					key;
	std::string					val;
	std::vector<datafold_type>	sub;
} datafold_t;

std::ostream & operator<< (std::ostream & o, std::vector<datafold_t> const & self);
std::ostream & operator<< (std::ostream & o, datafold_type const &);

class DataFold : public StringTools
{
	public:
		typedef std::vector<datafold_t> datavec;
	private:
		datavec	core;
		int		index;
	public:
		DataFold(std::string data);
		DataFold(void);
		DataFold(DataFold const & src);
		DataFold & operator= (DataFold const & rhs);
		~DataFold(void);
		void push_back(std::string key, std::string val);
		void push_back(std::string key, DataFold sub);
		size_t size() const;
		const datafold_type operator[] (size_t idx) const;
		const std::string operator[] (std::string) const;
		std::vector<datafold_t> getCore() const;
		int getIndex() const;
		operator datavec();
		DataFold parse_data(const std::string dst, std::string split_set);
		int df_type(std::string);
		int key_count(std::string key) const;
};

std::ostream & operator<< (std::ostream & o, DataFold const &);

#endif
