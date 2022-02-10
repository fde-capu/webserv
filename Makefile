# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/02/10 01:00:04 by fde-capu          #+#    #+#              #
#    Updated: 2022/02/10 01:12:36 by fde-capu         ###   ########.fr        #
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
all:		$(NAME)
$(NAME):	$(OBJS)
	$(CC) $(CCFLAGS) $(OBJS) -o $(NAME)
$(OBJS):	%.o : %.cpp $(HEAD)
	$(CC) $(CCFLAGS) -o $@ -c $<
clean:
	-rm -f $(OBJS)
fclean:		clean
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
	cd nginx-standalone && \
	make
nginx:		nginx-build
	cd test-confs && \
	pkill nginx
	$(PWD)/nginx-standalone/sandbox/sbin/nginx -c 42_tester.conf && \
	netstat -tunlp
nginx-t:	nginx
	./general_tests.sh
