/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CircularBuffer.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/11 13:52:01 by fde-capu          #+#    #+#             */
/*   Updated: 2022/08/03 14:20:21 by fde-capu         ###   ########.fr       */
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

# define CIRCULARBUFFER_SIZE (1048576 * 1)
# define CIRCULARBUFFER_LIMIT 104857600

class CircularBuffer
{
	private:
		const int fd;
		const size_t size;
		const size_t limit;
		const char* memory;
		char* head;
		bool eof;

		CircularBuffer();
		void mountMemory();
		void resetMemory();
		std::string& set_eof();
		std::string& unfinished();
		bool checkLimits() const;

	public:
		CircularBuffer(int);
		CircularBuffer(CircularBuffer const &);
		CircularBuffer& operator= (CircularBuffer const &);
		~CircularBuffer();

		size_t getSize() const;
		int getFd() const;
		bool ended() const;
		size_t length() const;

		std::string& receive_until_eof(size_t = 0);
		std::string& receive_at_most(size_t);
		std::string output;

		operator std::string();
};

std::ostream & operator<< (std::ostream &, CircularBuffer const &);

#endif
