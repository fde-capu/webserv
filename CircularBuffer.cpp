/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CircularBuffer.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/11 13:51:42 by fde-capu          #+#    #+#             */
/*   Updated: 2022/05/12 16:56:25 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CircularBuffer.hpp"
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
	memory = static_cast<char *>(std::malloc(size * sizeof(char)));
	resetMemory();
}

void CircularBuffer::resetMemory()
{
	memset(const_cast<char *>(memory), 0, size);
	head = const_cast<char *>(memory);
}

void CircularBuffer::set_eof()
{
	std::cout << "//EOF" << std::endl;
	resetMemory();
	eof = true;
}

void CircularBuffer::receive_until_eof()
{
	int bytes;

	bytes = recv(fd, const_cast<char *>(memory), size, 0);
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
