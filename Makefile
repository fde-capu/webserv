# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/01/05 21:07:02 by fde-capu          #+#    #+#              #
#    Updated: 2022/05/30 14:46:54 by fde-capu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME1	=	webserv
ARGS1	=	webserv-default.conf

NAME2	=	cgi_webserv
ARGS2	=	executable 123

DEBUG	=	1
ENVS	=	-DAGV_SKIP_CHECK=1
SRCS	=	strings.cpp FileString.cpp DataFold.cpp \
			StringTools.cpp ArgVal.cpp datafold_type.cpp WebServ.cpp \
			CircularBuffer.cpp WebServ_helpers.cpp CgiWrapper.cpp
SRCS1	=	main.cpp
SRCS2	=	main_cgi.cpp
HEAD	=	Makefile header.hpp \
			FileString.hpp DataFold.hpp StringTools.hpp ArgVal.hpp \
			datafold_type.hpp DataFold_defines.hpp bladefs.hpp WebServ.hpp \
			CircularBuffer.hpp CgiWrapper.hpp
SHELL	=	/bin/sh
CC		=	c++ -std=c++98 -Wfatal-errors -DVERBOSE=$(DEBUG) $(ENVS)
CCFLAGS	=	-Wall -Werror -Wextra -g -O0 -fno-limit-debug-info
OBJS	=	$(SRCS:.cpp=.o)
OBJS1	=	$(SRCS1:.cpp=.o)
OBJS2	=	$(SRCS2:.cpp=.o)
VAL		=	valgrind
VALFLAG	=	--tool=memcheck \
			--leak-check=full \
			--show-leak-kinds=all \
			--track-origins=yes \
			--show-reachable=yes
GDBSC	=	gdb.script
all:		line $(NAME1) $(NAME2)
ws:			line $(NAME1)
cgi:		line $(NAME2)
1:			ws
2:			cgi
line:
	@echo "\n************************\n"
$(NAME1):	$(OBJS1)
	$(CC) $(CCFLAGS) $(OBJS) $(OBJS1) -o $(NAME1)
$(NAME2):	$(OBJS2)
	$(CC) $(CCFLAGS) $(OBJS) $(OBJS2) -o $(NAME2)
$(OBJS):	%.o : %.cpp $(HEAD) $(SRCS)
	$(CC) $(CCFLAGS) -o $@ -c $<
$(OBJS1):	$(OBJS) $(SRCS1)
	$(CC) $(CCFLAGS) -o $(OBJS1) -c $(SRCS1)
$(OBJS2):	$(OBJS) $(SRCS2)
	$(CC) $(CCFLAGS) -o $(OBJS2) -c $(SRCS2)
clean:
	-rm -f $(OBJS1)
	-rm -f $(OBJS2)
fclean:		clean
	-rm -f $(NAME1)
	-rm -f $(NAME2)
re:			fclean all
rt:			re t
v1:			1
	$(VAL) ./$(NAME1) $(ARGS1)
v2:			2
	$(VAL) ./$(NAME2) $(ARGS2)
vf1:		1
	$(VAL) $(VALFLAG) ./$(NAME1) $(ARGS1)
vf2:		2
	$(VAL) $(VALFLAG) ./$(NAME2) $(ARGS2)
t1:			1
	./$(NAME1) $(ARGS1)
t2:			2
	./$(NAME2) $(ARGS2)
g1:			1
	gdb -x $(GDBSC) --args ./$(NAME1) $(ARGS1)
g2:			2
	gdb -x $(GDBSC) --args ./$(NAME2) $(ARGS2)
tg1:		1 pk
	./$(NAME1) $(ARGS1) &
	-./general_tests.sh
tg2:		2 pk
	./$(NAME2) $(ARGS2) &
	-./cgi_test.sh
pk:
	-pkill webserv
