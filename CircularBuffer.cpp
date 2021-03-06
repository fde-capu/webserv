/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CircularBuffer.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/11 13:51:42 by fde-capu          #+#    #+#             */
/*   Updated: 2022/08/03 14:31:30 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CircularBuffer.hpp"
#include "bladefs.hpp"
#include <iostream>
# include <errno.h>

CircularBuffer::CircularBuffer(int u_fd)
: fd(u_fd), size(CIRCULARBUFFER_SIZE), limit(CIRCULARBUFFER_LIMIT), eof(false)
{
	mountMemory();
	return ;
}

CircularBuffer::CircularBuffer(CircularBuffer const & src)
: fd(src.getFd()), size(src.getSize()), limit(CIRCULARBUFFER_LIMIT), eof(false)
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

std::string& CircularBuffer::unfinished()
{
	resetMemory();
	verbose(2) << "(CircularBuffer) Returning unfinished." << std::endl;
	return output;
}

bool CircularBuffer::checkLimits() const
{
	if (length() > limit)
		throw std::length_error("(CircularBuffer) Is limited.");
	return true;
}

std::string& CircularBuffer::receive_at_most(size_t max)
{
	static int V(2);
	static int O_LIM(15);
	size_t in_size;
	int bytes;

	while (checkLimits() || true)
	{
		verbose(V) << "(receive_at_most) " << max << \
			", Length: " << length() << \
			", capacity: " << output.capacity() << std::endl;

		in_size = max < size ? max : size;
		in_size = max > length() && max - length() < in_size ? \
			max - length() : in_size;

		verbose(V) << \
			"(receive_at_most) Actually receiving " << in_size << std::endl;

		if (length() > max)
			return set_eof();

		bytes = read(fd, const_cast<char *>(memory), in_size);

		verbose(V) << "(receive_at_most) got " << bytes << \
			", called " << in_size << ", max " << max << ", have " \
			<< length();

		if (bytes > 0 && bytes <= O_LIM)
		{
			verbose(V) << "\t(" << std::string(memory).substr(0, bytes) << ")";
		}
		else if (bytes > 0 && bytes > O_LIM)
		{
			verbose(V) << "\t(" << std::string(memory).substr(0, O_LIM) << \
				"...) len " << bytes;	
		}

		verbose(1) << "\r len:" << length() << "\terrno:" << (errno & EWOULDBLOCK);
		verbose(1) << "\t max:" << max;
		verbose(V) << std::endl;

		if (bytes == -1)
		{
			verbose(V) << "(receive_at_most) Try again: " << \
				strerror(errno) << "." << std::endl;
			continue ;
		}
		if (bytes == 0)
		{
			verbose(V) << "(receive_at_most) Zero bytes, EOF." << std::endl;
			return set_eof();
		}
		if (static_cast<size_t>(bytes) < in_size)
		{
			verbose(V) << \
				"(receive_at_most) Buffer less than expected, come again." \
				<< std::endl;
			output.append(memory, bytes);
			continue ;
		}
		if (static_cast<size_t>(bytes) == in_size)
		{
			verbose(V) << \
				"(receive_at_most) Buffer full, come again." << std::endl;
			output.append(memory, bytes);
			continue ;
		}
	}
}

std::string& CircularBuffer::receive_until_eof(size_t max)
{
	int bytes;

	while (checkLimits() || true)
	{
		if (max && length() >= max)
			return set_eof();

		bytes = read(fd, const_cast<char *>(memory), size);

		verbose(2) << "(receive_until_eof) ->" << fd << ": bytes " << bytes << \
			" size " << size << "\t(" << std::string(memory).substr(0, bytes) \
			<< ")" << std::endl;

		if (bytes == -1 && fd == 0) // stdin
		{
			return set_eof();
		}
		if (bytes == -1)
		{
			verbose(2) << "(receive_until_eof) Encontered an error, treated " << \
				"as warning (" << errno << "): " << strerror(errno) << std::endl;
			return unfinished();
		}
		if (bytes == 0)
		{
			verbose(2) << "(receive_until_eof) 0" << std::endl;
			return set_eof();
		}
		if (static_cast<size_t>(bytes) < size)
		{
			verbose(2) << "(receive_until_eof) " << bytes << " < " << size << std::endl;
			output.append(memory, bytes);
			continue ;
		}
		if (static_cast<size_t>(bytes) == size)
		{
			verbose(2) << "(receive_until_eof) " << bytes << " == " << size << std::endl;
			output.append(memory, size);
			continue ;
		}
	}
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

size_t CircularBuffer::length() const
{ return output.length(); }

CircularBuffer::operator std::string()
{ return receive_until_eof(); }
