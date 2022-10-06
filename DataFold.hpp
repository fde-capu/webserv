/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DataFold.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/06 18:40:12 by fde-capu          #+#    #+#             */
/*   Updated: 2022/10/06 23:53:23 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DATAFOLD_HPP
# define DATAFOLD_HPP

# include "StringTools.hpp"
# include "datafold_type.hpp"
# include "DataFold_defines.hpp"
# include <cstdlib>

class DataFold
{
	private:
		datavec	core;
		size_t	index;
		size_t	loop_index;

	public:
		typedef std::vector<int>			vint;
		typedef std::vector<std::string>	vstr;

		DataFold(std::string);
		DataFold();
		DataFold(DataFold const&);
		DataFold(datafold_t);
		DataFold(const std::vector<std::string> &);
		DataFold& operator= (DataFold const&);
		DataFold& operator= (const datafold_t &);
		~DataFold();
		void push_back(std::string, std::string);
		void push_back(std::string, const DataFold &);
		void push_back(datafold_t &);
		void push_back(const datafold_t &);
		void push_back(datavec &);
		void push_back(const datavec &);
		DataFold& pop();
		size_t size() const;
		const std::string operator[] (std::string) const;
		const datafold_type operator[] (size_t idx) const;
		datavec getCore() const;
		int getIndex() const;
		DataFold parse_data(const std::string dst);
		DataFold parse_only_val(const datafold_t &);
		int df_type(std::string);
		const std::string getFirstByKey(std::string key) const;
		std::string eqvals_to_arrstr(std::string key) const;
		std::string quoted_val(const datafold_t &) const;
		const std::string getValStr(std::string key) const;
		std::string get_val_str() const;
		bool has_key(std::string) const;
		int key_count(std::string) const;
		int key_count_exists_check(std::string key) const;
		void key_count_single_check(std::string key) const;
		void core_check() const;
		void string_check(const datafold_t &) const;
		void array_check(const datafold_t &) const;
		void not_sub_check(const datafold_t &) const;
		operator datavec() const;
		operator std::string() const;
		operator int() const;
		operator std::vector<std::string>() const;
		datafold_t get_datafold() const;
		datafold_t get_datafold(std::string, std::string) const;
		std::vector<int> get_vector_int(std::string) const;
		std::vector<int> get_vector_int(std::string, std::string) const;
		std::vector<std::string> get_vector_str() const;
		std::vector<std::string> get_vector_str(std::string) const;
		std::vector<std::string> get_vector_str(std::string, std::string) const;
		std::string clean_before_parse(std::string&) const;
		void array_into_inline(std::string&) const;
		bool empty() const;
		DataFold get_val(std::string) const;
		DataFold get_val(std::string, std::string) const;
		DataFold get_val(datafold_t) const;
		DataFold get_val() const;
		DataFold only_val() const;
		void set_only_val();
		std::string string() const;
		DataFold filter(std::string) const;
		void set(std::string, std::string);
		void set(const char*, const char*);

		std::string get(std::string) const;

		template <typename T>
		T get(std::string key) const
		{
			for (size_t i = 0; i < index; i++)
				if (key == core[i].key)
					return T(core[i]);
			return T();
		}

		template <>
		DataFold get(std::string key) const
		{
			DataFold out;
			for (size_t i = 0; i < index; i++)
				if (key == core[i].key)
					out.push_back(core[i]);
			return out;
		}

		template <typename T>
		T get(std::string key, std::string ksub) const
		{
			static_cast<void>(key);
			static_cast<void>(ksub);
			return T();
		}

		template <>
		std::vector<std::string> get(std::string key, std::string ksub) const
		{ return get_vector_str(key, ksub); }

		template <typename T>
		T last() const
		{ return T(core[index - 1]); } 

		bool loop();
		void loop_reset();
		bool loop_ended();
		bool not_ended();
		std::string key;
		std::string val;
		int type;
		bool is_single_array() const;
};

std::ostream& operator<< (std::ostream& o, DataFold const&);

#endif
