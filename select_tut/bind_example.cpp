/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bind_example.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 20:03:42 by fde-capu          #+#    #+#             */
/*   Updated: 2022/01/06 04:21:59 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>

#define MY_SOCK_PATH "./foo_socket"
#define LISTEN_BACKLOG 3490

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main()
{
	int sfd, cfd;
	struct sockaddr_un my_addr, peer_addr;
	socklen_t peer_addr_size;

	sfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sfd == -1)
		handle_error("socket");

	memset(&my_addr, 0, sizeof(struct sockaddr_un));

	/* Clear structure */
	my_addr.sun_family = AF_UNIX;
	strncpy(my_addr.sun_path, MY_SOCK_PATH, sizeof(my_addr.sun_path) - 1);

	std::cout << "sfd: " << sfd << ", my_addr: " << &my_addr << std::endl;
	if (bind(sfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_un)) == -1)
		handle_error("bind");

	if (listen(sfd, LISTEN_BACKLOG) == -1)
		handle_error("listen");

	/* Now we can accept incoming connections one
	   at a time using accept(2) */

	peer_addr_size = sizeof(struct sockaddr_un);
	cfd = accept(sfd, (struct sockaddr *) &peer_addr,
			&peer_addr_size);
	if (cfd == -1)
		handle_error("accept");

	/* Code to deal with incoming connection(s)... */

	/* When no longer required, the socket pathname, MY_SOCK_PATH
	   should be deleted using unlink(2) or remove(3) */
}
