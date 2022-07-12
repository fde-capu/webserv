/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CircularBuffer.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/11 13:52:01 by fde-capu          #+#    #+#             */
/*   Updated: 2022/07/12 14:55:44 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CIRCULARBUFFER_HPP
# define CIRCULARBUFFER_HPP

# include <iostream>
# include <cstring>
# include <unistd.h>
# include <cstdlib>
# include <sys/socket.h>
# include <vector>

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
		std::string& set_eof();

	public:
		CircularBuffer(int);
		CircularBuffer(CircularBuffer const &);
		CircularBuffer& operator= (CircularBuffer const &);
		~CircularBuffer();

		size_t getSize() const;
		int getFd() const;
		bool ended() const;

		std::string& receive_until_eof();
		std::string& receive_at_most(size_t);
		std::string output;

		operator std::string();
};

std::ostream & operator<< (std::ostream &, CircularBuffer const &);

#endif
