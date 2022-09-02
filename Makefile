# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/01/05 21:07:02 by fde-capu          #+#    #+#              #
#    Updated: 2022/09/02 17:29:25 by fde-capu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Test 2
# make ft
# t2 : Tests cgi-wrapper (made based on webserv).

MAKESILENT = 

NAME1	=	webserv
XARGS1	=	unit/webserv-unit.conf

NAME2	=	cgi_webserv
ARGS2	=	./unit/ubuntu_cgi_tester 9000

TEST_SH	=	./unit/general_tests.sh

DEBUG	=	1
ENVS	=	-DAGV_SKIP_CHECK=1

SRCS	=	FileString.cpp DataFold.cpp \
			StringTools.cpp ArgVal.cpp datafold_type.cpp \
			CircularBuffer.cpp WebServ_helpers.cpp \
			http_routes.cpp CgiWrapper.cpp Chronometer.cpp \
			WebServ_read_methods.cpp WebServ_proc_header.cpp \
			WebServ_operator_out.cpp WebServ_ports.cpp WebServ.cpp
SRCS1	=	main.cpp
SRCS2	=	main_cgi.cpp
HEAD	=	argval_ws.conf argval_cgi.conf header.hpp \
			FileString.hpp DataFold.hpp StringTools.hpp ArgVal.hpp \
			datafold_type.hpp DataFold_defines.hpp bladefs.hpp WebServ.hpp \
			CircularBuffer.hpp CgiWrapper.hpp Chronometer.hpp
SHELL	=	/bin/sh
CC		=	$(MAKESILENT) c++ -std=c++98 -Wfatal-errors -DVERBOSE=$(DEBUG) $(ENVS)
CCFLAGS	=	-Wall -Werror -Wextra -g -O0
OBJS	=	$(SRCS:.cpp=.o)
OBJS1	=	$(SRCS1:.cpp=.o)
OBJS2	=	$(SRCS2:.cpp=.o)
VAL		=	valgrind
VALFLAG	=	--tool=memcheck \
			--leak-check=full \
			--show-leak-kinds=all \
			--track-origins=yes \
			--show-reachable=yes
DOT		=	@echo -n ".";

all:		line $(NAME1) $(NAME2)
ws:			line $(NAME1)
cgi:		line $(NAME2)
1:			ws
2:			cgi
intro:		line
	@echo "Using CC CCFLAGS = $(CC) $(CCFLAGS)"
outro:		line
line:
	@echo "\n************************\n"
$(NAME1):	$(OBJS1) $(OBJS)
	$(CC) $(CCFLAGS) $(OBJS) $(OBJS1) -o $(NAME1)
$(NAME2):	$(OBJS2) $(OBJS)
	$(CC) $(CCFLAGS) $(OBJS) $(OBJS2) -o $(NAME2)
$(OBJS):	%.o : %.cpp $(HEAD)
	$(CC) $(CCFLAGS) -o $@ -c $<
$(OBJS1):	%.o : %.cpp
	$(DOT)
	$(CC) $(CCFLAGS) -o $@ -c $<
$(OBJS2):	%.o : %.cpp
	$(DOT)
	$(CC) $(CCFLAGS) -o $@ -c $<

#$(OBJS1):	intro $(OBJS) outro
#	$(CC) $(CCFLAGS) -o $(OBJS1) -c $(SRCS1)
#$(OBJS2):	intro $(OBJS) outro
#	$(CC) $(CCFLAGS) -o $(OBJS2) -c $(SRCS2)

clean:		lynx-clean
	-@rm -f $(OBJS)
	-@rm -f $(OBJS1)
	-@rm -f $(OBJS2)
fclean:		clean lynx-fclean
	-@rm -f $(NAME1)
	-@rm -f $(NAME2)
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
g1:			1
	gdb --args ./$(NAME1) $(ARGS1)
k1:
	-pkill $(NAME1)
k2:
	-pkill $(NAME2)
youpi:
	echo 'Initial YoupiBanane/youpi.bla' > unit/confs/html4242/YoupiBanane/youpi.bla
	ls -l unit/confs/html4242/YoupiBanane/youpi.bla

####### :::::::::: #######
####### :: lynx :: #######
####### :::::::::: #######

lynx:
	@[ -f lynx ] && exit 0 ||:
	@echo "\nBuilding lynx standalone..."
	@-cd lynx-standalone && \
	make -s > /dev/null 2> /dev/null
	@mv lynx-standalone/lynx .
lynx-fclean:
	@cd lynx-standalone && \
	make -s fclean
	@rm -f lynx
lynx-clean:
	@cd lynx-standalone && \
	make -s clean
lynx-re:
	cd lynx-standalone && \
	make re

####### :::::::::: #######
