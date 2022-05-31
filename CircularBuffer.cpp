/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CircularBuffer.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/11 13:51:42 by fde-capu          #+#    #+#             */
/*   Updated: 2022/05/31 11:58:08 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CircularBuffer.hpp"
#include "bladefs.hpp"
#include <iostream>

CircularBuffer::CircularBuffer(int u_fd)
: fd(u_fd), size(10), eof(false)
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

void CircularBuffer::set_eof()
{
	resetMemory();
	eof = true;
}

std::string CircularBuffer::reof_out()
{
	receive_until_eof();
	return output;
}

void CircularBuffer::receive_until_eof()
{
	int bytes;

	verbose(5) << "(CircularBuffer) Receiving from fd " << fd << "." << std::endl;
	bytes = recv(fd, const_cast<char *>(memory), size, 0);
	if (bytes == -1 && fd == 0) // stdin
		return set_eof();
	if (bytes == -1)
		throw std::domain_error("(CircularBuffer) Something went wrong receiving data.");
	if (bytes == 0)
		return set_eof();
	if (static_cast<size_t>(bytes) <= size)
		output += std::string(memory).substr(0, bytes);
	if (static_cast<size_t>(bytes) == size)
		return receive_until_eof();
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
