/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   datavec.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/16 11:47:13 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/16 13:26:17 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DATAVEC_HPP
# define DATAVEC_HPP

# include "datafold_type.hpp"
# include <sstream>
# include "DataFold_defines.hpp"
//# include <vector>

class datavec : public std::vector<datafold_t>
{
	public:
		operator std::string() const;
};

std::ostream & operator<< (std::ostream & o, std::vector<datafold_type> const &);

#endif
