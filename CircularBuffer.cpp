/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CircularBuffer.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/11 13:51:42 by fde-capu          #+#    #+#             */
/*   Updated: 2022/08/11 15:02:53 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CircularBuffer.hpp"
#include "bladefs.hpp"
#include <iostream>
# include <errno.h>

CircularBuffer::CircularBuffer(int u_fd)
: fd(u_fd), size(CIRCULARBUFFER_SIZE), limit(CIRCULARBUFFER_LIMIT), \
	eof(false), success(true)
{
	mountMemory();
	return ;
}

CircularBuffer::CircularBuffer(CircularBuffer const & src)
: fd(src.getFd()), size(src.getSize()), limit(CIRCULARBUFFER_LIMIT), \
	eof(false), success(true)
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
	success = false;
	return set_eof();
}

bool CircularBuffer::checkLimits(size_t& plus) const
{
	if (length() + plus > limit)
	{
		verbose(1) << "(CircularBuffer) Is limited so refuses to read more." << \
			std::endl;
		return false;
	}
	return true;
}

bool CircularBuffer::checkLimits() const
{
	if (length() > limit)
	{
		verbose(1) << "(CircularBuffer) Is limited so refuses to read more." << \
			std::endl;
		return false;
	}
	return true;
}

std::string& CircularBuffer::receive_exactly(size_t nbytes)
{
	size_t size_ini(output.length());
	size_t size_cur(0);
	size_t size_fin(size_ini + nbytes);
	int rbytes;

	if (checkLimits(nbytes))
		output.reserve(length() + nbytes);
	else
		return unfinished();
	while (size_cur < size_fin)
	{
		if (!checkLimits())
			return unfinished();
		std::cout << "\r(receive_exactly) " << length();
		rbytes = read(fd, const_cast<char *>(memory), size);
		if (rbytes == -1)
			continue ;
		if (rbytes == 0)
			return set_eof();
		if (static_cast<size_t>(rbytes) <= size)
		{
			output.append(memory, rbytes);
			size_cur += rbytes;
		}
	}
	return set_eof();
}

std::string& CircularBuffer::receive_until_eof()
{
	int bytes;

	while (1)
	{
		if (!checkLimits())
			return unfinished();
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

bool CircularBuffer::fail() const
{ return !success; }

size_t CircularBuffer::length() const
{ return output.length(); }

CircularBuffer::operator std::string()
{ return receive_until_eof(); }
