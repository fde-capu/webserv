# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/02/10 01:00:04 by fde-capu          #+#    #+#              #
#    Updated: 2022/05/24 01:20:56 by fde-capu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SERVER	=	127.0.0.1
NAME	=	webserv
SRCS	=	
HEAD	=	Makefile
SHELL	=	/bin/bash
CC		=	clang++ -std=c++98
CCFLAGS	=	-Wall -Werror -Wextra -g
OBJS	=	$(SRCS:.cpp=.o)
VAL		=	valgrind
VALFLAG	=	--tool=memcheck \
			--leak-check=full \
			--show-leak-kinds=all \
			--track-origins=yes \
			--show-reachable=yes
NGINX	=	$(PWD)/nginx-standalone/sandbox/sbin/nginx
CONF	=	nginx-as-webserv.conf
SRCME	=	@echo 'Please run: `source ./source-me-after-install-rc`.';
MAKEFLAGS =	--no-print-directory
all:		nginx-build lynx
	$(SRCME)
$(NAME):	$(OBJS)
	$(CC) $(CCFLAGS) $(OBJS) -o $(NAME)
$(OBJS):	%.o : %.cpp $(HEAD)
	$(CC) $(CCFLAGS) -o $@ -c $<
clean:		nginx-clean lynx-clean
	-rm -f $(OBJS)
fclean:		clean nginx-fclean lynx-fclean
	-rm -f $(NAME)
re:			fclean all
nginx-build:
	@echo 'nginx: '
	-@cd nginx-standalone && \
	make
nginx:		nginx-build $(CONF)
	-pkill nginx
	cd confs && $(NGINX) -c $(CONF)
	netstat -tnl || ss -tnl
$(CONF):	nginx-r
t:			nginx-t
nginx-t:	nginx
	../general_tests.sh
nginx-r:	nginx
nginx-rt:	nginx-r nginx-t
nginx-tc:	$(CONF)
	$(NGINX) -c $(CONF) -t
nginx-clean:
	cd nginx-standalone && \
	make clean
nginx-fclean:
	cd nginx-standalone && \
	make fclean
nginx-re:
	cd nginx-standalone && \
	make re
nginx-relog:
	cd nginx-standalone/sandbox/logs && \
	echo '' > error.log && \
	echo '' > access.log
nginx-catlog:
	cd nginx-standalone/sandbox/logs && \
	cat access.log && cat error.log
nginx-tlog: nginx-relog nginx-t nginx-catlog
lynx:
	@echo 'lynx: '
	@-cd lynx-standalone && \
	make
lynx-fclean:
	cd lynx-standalone && \
	make fclean
lynx-clean:
	cd lynx-standalone && \
	make clean
lynx-re:
	cd lynx-standalone && \
	make re
lynx-nginx: lynx nginx
lynx-nginx-t: lynx nginx-t
g:	nginx-log
