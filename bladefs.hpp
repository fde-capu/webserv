/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bladefs.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/18 14:32:59 by fde-capu          #+#    #+#             */
/*   Updated: 2022/03/18 16:13:59 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BLADEFS_HPP
# define BLADEFS_HPP

# include <iostream>

typedef datafold_t df_t;
typedef std::string str_t;
typedef std::stringstream sstr;
typedef std::ostream ostr;

# define stool StringTools()
# define verbose(X) if (VERBOSE > X) std::cout
# define bad_arg std::invalid_argument
# define xo std::cout
# define xx std::cout
# define nl std::endl
# define X(X) xo << X << nl;
# define DF DataFold
# define nopos std::string::npos

#endif
