/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   datafold_type.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 11:46:22 by fde-capu          #+#    #+#             */
/*   Updated: 2022/10/06 23:17:57 by fde-capu         ###   ########.fr       */
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
		std::string string() const;
} dvec;
std::ostream& operator<< (std::ostream& o, datavec const&);

typedef struct datafold_type
{
	int			index;
	int			type;
	std::string	key;
	std::string	val;
	datavec		sub;

	datafold_type();
	operator std::string() const;
	operator int() const;

} datafold_t;

# include "bladefs.hpp"
#endif
