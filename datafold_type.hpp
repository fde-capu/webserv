/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   datafold_type.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 11:46:22 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/20 00:38:42 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DATAFOLD_TYPE_HPP
# define DATAFOLD_TYPE_HPP

# include "StringTools.hpp"
# include <string>
# include <vector>
# include "DataFold_defines.hpp"
# include <sstream>
# include <cstdlib>
# include <iostream>

typedef struct datafold_type datafold_t;

typedef class datavec : public std::vector<datafold_t>
{
	private:
		int loop_index;

	public:
		datavec();
		operator std::string() const;
		bool loop();
		void loop_reset();
		bool not_ended();
		std::string key;
		std::string val;
		int type;
} dvec;

std::ostream & operator<< (std::ostream & o, dvec const&);

typedef struct datafold_type
{
	int			index;
	int			type;
	std::string	key;
	std::string	val;
	datavec		sub;

	operator std::string() const;
	operator int() const;

	void log_self() const;

} datafold_t;

# include "bladefs.hpp"

ostr & operator<< (ostr & o, df_t const &);
ostr & operator<< (ostr & o, dvec const &);

#endif
