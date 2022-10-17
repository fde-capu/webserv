/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CircularBuffer.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/11 13:51:42 by fde-capu          #+#    #+#             */
/*   Updated: 2022/10/18 01:02:40 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CircularBuffer.hpp"
#include "bladefs.hpp"
#include <iostream>

CircularBuffer::CircularBuffer(int u_fd)
: fd(u_fd), size(CIRCULARBUFFER_SIZE), limit(CIRCULARBUFFER_LIMIT), \
	eof(false), success(true)
{
	mountMemory();
}

CircularBuffer::CircularBuffer(CircularBuffer const & src)
: fd(src.getFd()), size(src.getSize()), limit(CIRCULARBUFFER_LIMIT), \
	eof(false), success(true)
{
	*this = src;
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
	verbose(5) << "(CircularBuffer) Returning unfinished/unknown " << length() << std::endl;
	return output;
}

std::string& CircularBuffer::out_of_resource()
{
	verbose(2) << "(CircularBuffer) Returning out_of_resource." << std::endl;
	success = false;
	return set_eof();
}

bool CircularBuffer::outOfLimits(const size_t& plus) const
{
	if (length() + plus > limit)
	{
		verbose(CRITICAL) << "(CircularBuffer) Limited to " << limit \
			<< ", refuses to read more (" << length() << " and wants " \
			<< plus << ")." << std::endl;
		return true;
	}
	return false;
}

std::string& CircularBuffer::try_to_receive(const size_t nbytes, bool slow)
{
	int V(3);
	size_t size_ini(output.length());
	size_t size_cur(0);
	size_t size_fin(size_ini + nbytes);
	int rbytes;
	Chronometer time_out;
	size_t time_gap;

	verbose(V) << "(CircularBuffer) try_to_receive " << nbytes << std::endl;
	if (outOfLimits(nbytes))
	{
		verbose(V) << "(CircularBuffer) " << size_fin << " would extrapolate limits." << std::endl;
		return out_of_resource();
	}
	output.reserve(length() + nbytes);
	time_gap = CIRCULARBUFFER_TIMEOUT_MSEC * (slow ? 1 : 1);
	verbose(V) << "(CircularBuffer) Started: " << size_ini << std::endl;
	while (size_cur < size_fin)
	{
		if (time_out > time_gap)
		{
			verbose(V) << "(CircularBuffer) Timeout! time_gap " << time_gap << std::endl;
			return unfinished();
		}
		rbytes = read(fd, const_cast<char *>(memory), nbytes);
		if (rbytes <= -1)
			continue ;
		if (rbytes == 0)
			return set_eof();
		if (outOfLimits(rbytes))
			return out_of_resource();
		if (static_cast<size_t>(rbytes) <= size)
		{
			verbose(V) << "(CircularBuffer) TRY Appending " << rbytes << "->" << (length() + rbytes) << std::endl;
			output.append(memory, rbytes);
			size_cur += rbytes;
			time_out.btn_reset();
		}
	}
	verbose(V) << "(CircularBuffer) Current " << size_cur << " >= " << size_fin << " final." << std::endl;
	return unfinished();
}

std::string& CircularBuffer::receive_until(const std::string& match)
{
	size_t look;
	Chronometer time_out;

	while (time_out <= CIRCULARBUFFER_TIMEOUT_MSEC)
	{
		try_to_receive(1, true);
		if (output.length() < match.length())
			continue ;
		look = output.length() - match.length();
		if (output.find(match, look) != 0)
			continue ;
		break ;
	}
	return output;
}

std::string& CircularBuffer::receive_until_eof()
{
	int V(3);
	int rbytes;
	Chronometer time_out;

	verbose(V) << "(CircularBuffer) receive_until_eof." << std::endl;
	while (1)
	{
		if (time_out > CIRCULARBUFFER_TIMEOUT_MSEC)
		{
			verbose(V) << "(CircularBuffer) EOF timeout!" << std::endl;
			return unfinished();
		}
		rbytes = read(fd, const_cast<char *>(memory), size);
		if (rbytes <= -1 && fd == 0) // stdin
			return set_eof();
		if (rbytes <= -1)
			continue ;
		if (rbytes == 0)
			return set_eof();
		if (outOfLimits(rbytes))
			return out_of_resource();
		if (static_cast<size_t>(rbytes) <= size)
		{
			verbose(V) << "(CircularBuffer) EOF Appending " << rbytes << "->" << (length() + rbytes) << std::endl;
			output.append(memory, rbytes);
			time_out.btn_reset();
			continue ;
		}
	}
}

CircularBuffer & CircularBuffer::operator= (CircularBuffer const & rhs)
{
	if (this != &rhs)
		mountMemory();
	return *this;
}

std::ostream & operator<< (std::ostream & o, CircularBuffer const & self)
{
	o << self.output;
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
