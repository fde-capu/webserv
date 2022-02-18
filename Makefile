# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/01/05 21:07:02 by fde-capu          #+#    #+#              #
#    Updated: 2022/02/18 17:41:14 by fde-capu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	=	webserv
SRCS	=	main.cpp Encapsulator.cpp
HEAD	=	Makefile Encapsulator.hpp defines.hpp setup.hpp
SHELL	=	/bin/sh
CC		=	clang++ -std=c++98 -Wfatal-errors
CCFLAGS	=	-Wall -Werror -Wextra -g
OBJS	=	$(SRCS:.cpp=.o)
VAL		=	valgrind
VALFLAG	=	--tool=memcheck \
			--leak-check=full \
			--show-leak-kinds=all \
			--track-origins=yes \
			--show-reachable=yes
LINE	=	@echo "\n************************\n"
all:		line $(NAME)
line:
	$(LINE)
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
tserver:	all pk
	./$(NAME)
t:			all pk
	./$(NAME) &
	-./general_tests.sh
v:			all
	$(VAL) ./$(NAME)
pk:
	-pkill webserv
