# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/02/10 01:00:04 by fde-capu          #+#    #+#              #
#    Updated: 2022/02/10 02:43:27 by fde-capu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

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
all:		$(NAME)
$(NAME):	$(OBJS)
	$(CC) $(CCFLAGS) $(OBJS) -o $(NAME)
$(OBJS):	%.o : %.cpp $(HEAD)
	$(CC) $(CCFLAGS) -o $@ -c $<
clean:		nginx-clean
	-rm -f $(OBJS)
fclean:		clean nginx-fclean
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
	-cd nginx-standalone && \
	make && \
	source source-me-after-install-rc
nginx:		nginx-build
	cd test-confs && \
	pkill nginx
	$(NGINX) -c $(CONF) && \
	netstat -tunlp
nginx-t:	nginx
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
