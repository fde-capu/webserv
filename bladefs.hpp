/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bladefs.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/18 14:32:59 by fde-capu          #+#    #+#             */
/*   Updated: 2022/08/29 22:08:27 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BLADEFS_HPP
# define BLADEFS_HPP

# include <iostream>
# include <csignal>

# define verbose(X) if (VERBOSE >= X) std::cout
# define BREAK raise(SIGTRAP);
# define SHORT(X) X.substr(0, VLIM) << "(" << X.length() << ")"

#endif
