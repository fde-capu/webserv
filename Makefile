# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/01/05 21:07:02 by fde-capu          #+#    #+#              #
#    Updated: 2022/06/24 14:23:16 by fde-capu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME1	=	webserv
ARGS1	=	unit/webserv-unit.conf

NAME2	=	cgi_webserv
ARGS2	=	./unit/ubuntu_cgi_tester 9000

TEST_SH	=	./unit/general_tests.sh

DEBUG	=	1
ENVS	=	-DAGV_SKIP_CHECK=1

SRCS	=	FileString.cpp DataFold.cpp \
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
$(OBJS):	%.o : %.cpp $(HEAD)
	$(CC) $(CCFLAGS) -o $@ -c $<
$(OBJS1):	$(OBJS)
	$(CC) $(CCFLAGS) -o $(OBJS1) -c $(SRCS1)
$(OBJS2):	$(OBJS)
	$(CC) $(CCFLAGS) -o $(OBJS2) -c $(SRCS2)
clean:
	-rm -f $(OBJS)
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
	@echo '=======> Please run unit/general_tests.sh on another terminal <======='
	./$(NAME1) $(ARGS1)
t2:			2
	./$(NAME2) $(ARGS2)
ftcgi:		t2
g1:			1
	gdb --args ./$(NAME1) $(ARGS1)
g2:			2
	gdb --args ./$(NAME2) $(ARGS2)
gt1:
	$(TEST_SH)
gt2:
	./cgi_test.sh
k1:
	-pkill $(NAME1)
k2:
	-pkill $(NAME2)
ft:
	@echo "Please run \`make ftcgi\` on another terminal.";
	@echo "";
	unit/ubuntu_tester http://0.0.0.0:4242

.PHONY: webserv
webserv:	ws
webserv-up:	ws
gws:		g1
ws-t:		t1
wst:		t1
