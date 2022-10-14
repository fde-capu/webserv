/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CircularBuffer.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/11 13:52:01 by fde-capu          #+#    #+#             */
/*   Updated: 2022/10/15 01:36:28 by fde-capu         ###   ########.fr       */
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
# include "Chronometer.hpp"

# define CIRCULARBUFFER_TIMEOUT_MSEC 50
# define CIRCULARBUFFER_SIZE 10485760
# define CIRCULARBUFFER_LIMIT 52428800

class CircularBuffer
{
	private:
		const int fd;
		const size_t size;
		const size_t limit;
		const char* memory;
		char* head;
		bool eof;
		bool success;

		CircularBuffer();
		void mountMemory();
		void resetMemory();
		std::string& set_eof();
		std::string& out_of_resource();
		std::string& unfinished();
		bool outOfLimits(const size_t& = 0) const;

	public:
		CircularBuffer(int);
		CircularBuffer(CircularBuffer const &);
		CircularBuffer& operator= (CircularBuffer const &);
		~CircularBuffer();

		size_t getSize() const;
		int getFd() const;
		bool ended() const;
		size_t length() const;
		bool fail() const;

		std::string& receive_until(const std::string& = "\r\n");
		std::string& receive_until_eof();
		std::string& try_to_receive(const size_t = CIRCULARBUFFER_SIZE, bool = false);
		std::string output;

		operator std::string();
};

std::ostream & operator<< (std::ostream &, CircularBuffer const &);

#endif
