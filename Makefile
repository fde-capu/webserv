# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/01/05 21:07:02 by fde-capu          #+#    #+#              #
#    Updated: 2022/05/26 14:22:18 by fde-capu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	=	webserv
ARGS	=	webserv-default.conf
NAME2	=	cgi_webserv
ARGS2	=	executable
DEBUG	=	1
ENVS	=	-DAGV_SKIP_CHECK=1
SRCS	=	strings.cpp FileString.cpp DataFold.cpp \
			StringTools.cpp ArgVal.cpp datafold_type.cpp WebServ.cpp \
			CircularBuffer.cpp WebServ_helpers.cpp CgiWrapper.cpp
SRCS1	=	$(SRCS) main.cpp
SRCS2	=	$(SRCS) main_cgi.cpp
HEAD	=	Makefile header.hpp \
			FileString.hpp DataFold.hpp StringTools.hpp ArgVal.hpp \
			datafold_type.hpp DataFold_defines.hpp bladefs.hpp WebServ.hpp \
			CircularBuffer.hpp CgiWrapper.hpp
SHELL	=	/bin/sh
CC		=	c++ -std=c++98 -Wfatal-errors -DVERBOSE=$(DEBUG) $(ENVS)
CCFLAGS	=	-Wall -Werror -Wextra -g -O0 -fno-limit-debug-info
OBJS	=	$(SRCS1:.cpp=.o)
OBJS2	=	$(SRCS2:.cpp=.o)
VAL		=	valgrind
VALFLAG	=	--tool=memcheck \
			--leak-check=full \
			--show-leak-kinds=all \
			--track-origins=yes \
			--show-reachable=yes
GDBSC	=	gdb.script
LINE	=	@echo "\n************************\n"
all:		line $(NAME) $(NAME2)
webserv:	line $(NAME)
cgi:		line $(NAME2)
2:			cgi
line:
	$(LINE)
$(NAME):	$(OBJS)
	$(CC) $(CCFLAGS) $(OBJS) -o $(NAME)
$(NAME2):	$(OBJS2)
	$(CC) $(CCFLAGS) $(OBJS2) -o $(NAME2)
$(OBJS):	%.o : %.cpp $(HEAD)
	$(CC) $(CCFLAGS) -o $@ -c $<
clean:
	-rm -f $(OBJS)
	-rm -f $(OBJS2)
fclean:		clean
	-rm -f $(NAME)
	-rm -f $(NAME2)
re:			fclean all
rt:			re t
tserver:	all pk
	./$(NAME) $(ARGS)
t:			webserv
	./$(NAME) $(ARGS)
t2:			cgi
	./$(NAME2) $(ARGS)
tg:			webserv pk
	./$(NAME) $(ARGS) &
	-./general_tests.sh
tg2:		cgi pk
	./$(NAME2) $(ARGS) &
	-./cgi_test.sh
v:			webserv
	$(VAL) ./$(NAME) $(ARGS)
v2:			cgi
	$(VAL) ./$(NAME2) $(ARGS)
vf:			webserv
	$(VAL) $(VALFLAG) ./$(NAME) $(ARGS)
vf2:		cgi
	$(VAL) $(VALFLAG) ./$(NAME2) $(ARGS2)
g:			webserv
	gdb -x $(GDBSC) --args ./$(NAME) $(ARGS)
g2:			cgi
	gdb -x $(GDBSC) --args ./$(NAME2) $(ARGS2)
pk:
	-pkill webserv
