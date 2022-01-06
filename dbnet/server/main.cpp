/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 21:07:26 by fde-capu          #+#    #+#             */
/*   Updated: 2022/01/06 18:28:06 by fde-capu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include <cstring>
#include <poll.h>
#include <iostream>
#include "Encapsulator.hpp"
#include "defines.hpp"
#include "setup.hpp"

void sigchld_handler(int s)
{
	(void)s;
	g_capsule->~Encapsulator();
	std::cout NL;
	std::cout TURN_OFF;
	std::cout THANK_YOU;
	exit(0);
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void die(const char * motive)
{
	perror(motive);
	exit(1);
}

void do_or_die(const int x, const char * motive)
{
	if (x == -1)
		die(motive);
}

void signals_up()
{
	struct sigaction sa;
	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;
	do_or_die(sigaction(SIGINT, &sa, NULL), "sigaction");
	do_or_die(sigaction(SIGQUIT, &sa, NULL), "sigaction");
	do_or_die(sigaction(SIGTSTP, &sa, NULL), "sigaction");
}

int main(void)
{
	int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;

	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;

	int yes=1;
//	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
						p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
					sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)
	{
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	std::cout WELLCOME;
	Encapsulator response("Wabadaba!\n");
	g_capsule = &response;
	int nfds = 1;
	struct pollfd pfds[nfds];
	pfds[0].fd = sockfd;
	pfds[0].events = POLLIN;
	int num_events;

	signals_up();

	std::cout RUNNING;

	while (1)
	{
		do_or_die(num_events = poll(pfds, nfds, 0), "poll");

		if (!num_events)
			continue ;

		do_or_die(listen(pfds[0].fd, BACKLOG), "listen");

		sin_size = sizeof their_addr;
		do_or_die(new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size), "accept");

//		inet_ntop(their_addr.ss_family,
//				get_in_addr((struct sockaddr *)&their_addr),
//				s, sizeof s);
//		printf("server: got connection from %s\n", s);

		do_or_die(send(new_fd, response.encapsulate(), response.full_length(), 0), "send");

		close(new_fd);

	} // listen + accept loop

	return 0;
}
