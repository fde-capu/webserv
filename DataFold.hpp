/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DataFold.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/06 18:40:12 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/16 13:25:26 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DATAFOLD_HPP
# define DATAFOLD_HPP

# include "datavec.hpp"
# include "StringTools.hpp"
# include "DataFold_defines.hpp"
# include <cstdlib>
//# include <iostream>
//# include <vector>
//# include <utility>
//# include "datafold_type.hpp"

class DataFold : public StringTools
{
	private:
		datavec	core;
		int		index;

	public:
		typedef std::vector<int>			vint;
		typedef std::vector<std::string>	vstr;

		DataFold(std::string);
		DataFold();
		DataFold(DataFold const & src);
		DataFold & operator= (DataFold const & rhs);
		~DataFold();
		void push_back(std::string key, std::string val);
		void push_back(std::string key, DataFold sub);
		size_t size() const;
		const std::string operator[] (std::string) const;
		const datafold_type operator[] (size_t idx) const;
		datavec getCore() const;
		int getIndex() const;
		DataFold parse_data(const std::string dst);
		int df_type(std::string);
		const std::string getFirstByKey(std::string key) const;
		std::string eqvals_to_arrstr(std::string key) const;
		std::string quoted_val(datafold_t) const;
		const std::string getValStr(std::string key) const;
		int key_count(std::string key) const;
		void key_count_exists_check(std::string key) const;
		void key_count_single_check(std::string key) const;
		operator datavec() const;
		operator std::string() const;
		datafold_t get_datafold(std::string) const;
		datafold_t get_datafold(std::string, std::string) const;
		std::vector<int> get_vector_int(std::string) const;
		std::vector<int> get_vector_int(std::string, std::string) const;
		std::vector<std::string> get_vector_str(std::string) const;
		std::vector<std::string> get_vector_str(std::string, std::string) const;
		void core_check() const;

		template <typename T>
		T get(std::string key) const
		{
			core_check();
			key_count_single_check(key);
			for (int i = 0; i < index; i++)
				if (key == core[i].key)
					return T(core[i]);
			return T();
		}
};

std::ostream & operator<< (std::ostream & o, DataFold const &);

#endif
