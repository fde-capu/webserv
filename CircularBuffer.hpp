/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CircularBuffer.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/11 13:52:01 by fde-capu          #+#    #+#             */
/*   Updated: 2022/05/12 16:51:41 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CIRCULARBUFFER_HPP
# define CIRCULARBUFFER_HPP

# include <iostream>
# include <cstring>
# include <sys/socket.h>
# include <cstdlib>

class CircularBuffer
{
	private:
		const int fd;
		const size_t size;
		const char* memory;
		char* head;
		bool eof;

		CircularBuffer();
		void mountMemory();
		void resetMemory();
		void set_eof();

	public:
		CircularBuffer(int);
		CircularBuffer(CircularBuffer const &);
		CircularBuffer& operator= (CircularBuffer const &);
		~CircularBuffer();

		size_t getSize() const;
		int getFd() const;

		std::string output;
		void receive_until_eof();
		bool ended() const;
};

std::ostream & operator<< (std::ostream &, CircularBuffer const &);

#endif
