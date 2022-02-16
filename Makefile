# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/02/10 01:00:04 by fde-capu          #+#    #+#              #
#    Updated: 2022/02/16 14:25:12 by fde-capu         ###   ########.fr        #
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
CONF	=	42_tester.conf
MAKEFLAGS =	--no-print-directory
#all:		$(NAME)
all:		nginx-build lynx
	@echo 'Please run: `source ./source-me-after-install-rc`.'
$(NAME):	$(OBJS)
	$(CC) $(CCFLAGS) $(OBJS) -o $(NAME)
$(OBJS):	%.o : %.cpp $(HEAD)
	$(CC) $(CCFLAGS) -o $@ -c $<
clean:		nginx-clean lynx-clean
	-rm -f $(OBJS)
fclean:		clean nginx-fclean lynx-fclean
	-rm -f $(NAME)
re:			fclean all
rt:			re t
vf:			all
	$(VAL) $(VALFLAG) ./$(NAME)
t:			all
	./$(NAME)
v:			all
	$(VAL) ./$(NAME)
nginx-build:
	@echo 'nginx: '
	-@cd nginx-standalone && \
	make
	-@source ./source-me-after-install-rc
nginx:		nginx-build
	-cd test-confs && \
	pkill nginx
	$(NGINX) -c $(CONF) && \
	netstat -tunlp
nginx-t:	nginx nginx-r
	./general_tests.sh
nginx-r:
	$(NGINX) -s reload
	netstat -tunlp
nginx-rt:	nginx-r
	./general_tests.sh
nginx-tc:
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
nginx-vimlog:
	cd nginx-standalone/sandbox/logs && \
	cat access.log && cat error.log

nginx-log: nginx-relog nginx-t nginx-vimlog
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
42:	all
	./ubuntu_tester http://$(SERVER):4242
