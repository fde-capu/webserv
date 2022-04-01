# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/01/05 21:07:02 by fde-capu          #+#    #+#              #
#    Updated: 2022/04/01 18:17:06 by fde-capu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	=	webserv
ARGS	=	webserv-default.conf
DEBUG	=	3
SRCS	=	main.cpp Encapsulator.cpp strings.cpp FileString.cpp DataFold.cpp \
			StringTools.cpp ArgVal.cpp datafold_type.cpp
HEAD	=	Makefile Encapsulator.hpp setup.hpp header.hpp \
			FileString.hpp DataFold.hpp StringTools.hpp ArgVal.hpp \
			datafold_type.hpp DataFold_defines.hpp bladefs.hpp
SHELL	=	/bin/sh
CC		=	c++ -std=c++98 -Wfatal-errors -DVERBOSE=$(DEBUG)
CCFLAGS	=	-Wall -Werror -Wextra -g -O0 -fno-limit-debug-info
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
	$(VAL) $(VALFLAG) ./$(NAME) $(ARGS)
tserver:	all pk
	./$(NAME) $(ARGS)
t:			all
	./$(NAME) $(ARGS)
tg:			all pk
	./$(NAME) $(ARGS) &
	-./general_tests.sh
v:			all
	$(VAL) ./$(NAME) $(ARGS)
pk:
	-pkill webserv
g:			all
	gdb --args ./$(NAME) $(ARGS)
