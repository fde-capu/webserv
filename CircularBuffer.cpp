/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CircularBuffer.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/11 13:51:42 by fde-capu          #+#    #+#             */
/*   Updated: 2022/07/21 13:21:04 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CircularBuffer.hpp"
#include "bladefs.hpp"
#include <iostream>
# include <errno.h>

CircularBuffer::CircularBuffer(int u_fd)
: fd(u_fd), size(CIRCULAR_BUFFER_SIZE), eof(false)
{
	mountMemory();
	return ;
}

CircularBuffer::CircularBuffer(CircularBuffer const & src)
: fd(src.getFd()), size(src.getSize()), eof(false)
{
	*this = src;
	return ;
}

void CircularBuffer::mountMemory()
{
	memory = static_cast<char *>(std::malloc(size * sizeof(char) + 1));
	resetMemory();
}

void CircularBuffer::resetMemory()
{
	memset(const_cast<char *>(memory), 0, size + 1);
	head = const_cast<char *>(memory);
}

std::string& CircularBuffer::set_eof()
{
	resetMemory();
	eof = true;
	verbose(2) << "(CircularBuffer) EOF set." << std::endl;
	return output;
}

std::string& CircularBuffer::receive_at_most(size_t max)
{
	size_t in_size;
	int bytes;

	while (1)
	{
		in_size = max < size ? max : size;
		in_size = max - output.length() < in_size ? max - output.length() : in_size;
		
		bytes = read(fd, const_cast<char *>(memory), in_size);

		verbose(1) << "(receive_at_most) fd " << fd << ": read " << bytes << \
			", in_size " << in_size << ", max " << max << ", have " \
			<< output.length() << "\t(" << \
			std::string(memory).substr(0, bytes) << ")" << std::endl;

		if (bytes == -1)
		{
			verbose(1) << "(receive_at_most) Try again: " << \
				strerror(errno) << "." << std::endl;
			continue ;
//			return set_eof();
		}
		if (bytes == 0)
		{
			verbose(1) << "(receive_at_most) Zero bytes, EOF." << std::endl;
			return set_eof();
		}
		if (static_cast<size_t>(bytes) < in_size)
		{
			verbose(1) << "(receive_at_most) Buffer lesser than expected, come again." \
				<< std::endl;
			output.append(memory, bytes);
			continue ;
		}
		if (static_cast<size_t>(bytes) == size)
		{
			verbose(1) << "(receive_at_most) Buffer full, come again." << std::endl;
			output.append(memory, bytes);
			continue ;
		}

		verbose (1) << "(receive_at_most) End of in data." << std::endl;
		output.append(memory, bytes);
		return set_eof();
	}
}

std::string& CircularBuffer::receive_until_eof()
{
	int bytes = read(fd, const_cast<char *>(memory), size);

	verbose(5) << "(CircularBuffer) ->" << fd << ": bytes " << bytes << \
		" size " << size << "\t(" << std::string(memory).substr(0, bytes) \
		<< ")" << std::endl;

	if (bytes == -1 && fd == 0) // stdin
	{
		set_eof();
		return output;
	}
	if (bytes == -1)
	{
		verbose(2) << "(CircularBuffer) Encontered an error, treated " << \
			"as warning, set point as EOF: " << strerror(errno) << std::endl;
		set_eof();
		return output;
	}
	if (bytes == 0)
		return receive_until_eof();
	if (static_cast<size_t>(bytes) < size)
	{
		output.append(memory, bytes);
		return receive_until_eof();
	}
	if (static_cast<size_t>(bytes) == size)
	{
		output.append(memory, size);
		return receive_until_eof();
	}
	return output;
}

CircularBuffer & CircularBuffer::operator= (CircularBuffer const & rhs)
{
	if (this != &rhs)
	{
		mountMemory();
	}
	return *this;
}

std::ostream & operator<< (std::ostream & o, CircularBuffer const & self)
{
	o << "::CircularBuffer::" << std::endl;
	static_cast<void>(self);
	return o;
}

CircularBuffer::~CircularBuffer()
{
	std::free(const_cast<char *>(memory));
	return ;
}

size_t CircularBuffer::getSize() const
{ return size; }

int CircularBuffer::getFd() const
{ return fd; }

bool CircularBuffer::ended() const
{ return eof; }

CircularBuffer::operator std::string()
{ return receive_until_eof(); }
