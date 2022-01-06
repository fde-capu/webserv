# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/01/05 19:21:44 by fde-capu          #+#    #+#              #
#    Updated: 2022/01/06 04:21:29 by fde-capu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	=	fwd
XARGS	=	3490 2 "127.0.0.1"
ARGS	=	
SRCS	=	bind_example.cpp
HEAD	=	Makefile
SHELL	=	/bin/sh
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
t:			all
	-./$(NAME) $(ARGS)
v:			all
	-$(VAL) ./$(NAME) $(ARGS)
vf:			all
	-$(VAL) $(VALFLAG) ./$(NAME) $(ARGS)

