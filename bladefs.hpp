/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bladefs.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/18 14:32:59 by fde-capu          #+#    #+#             */
/*   Updated: 2022/10/05 20:45:46 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BLADEFS_HPP
# define BLADEFS_HPP

# include <iostream>
# include <csignal>

# define verbose(X) if (VERBOSE >= X) std::cout
# define BREAK raise(SIGTRAP);
# define SHORT(X) X.substr(0, VERBOSE_LINE_LIMIT ) << " (" << X.length() << ")"
# define LONG(X) X.substr(0, VERBOSE_LINE_LIMIT  * 10) << " (" << X.length() << ")"
# define HELLO_WORLD "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Length: 15\r\n\r\nHello, world!\r\n"
# define CRITICAL 1

#endif
