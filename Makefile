# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/01/05 21:07:02 by fde-capu          #+#    #+#              #
#    Updated: 2022/10/28 22:45:20 by fde-capu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

MAKESILENT	  = @
VERBOSE		  =	3
ENVS		  =	-DAGV_SKIP_CHECK=0
TRACKMAKEFILE = Makefile

NAME1	=	webserv
ARGS1	=	unit/webserv-unit.conf

SRCS	=	FileString.cpp DataFold.cpp \
			StringTools.cpp ArgVal.cpp datafold_type.cpp \
			WebServ_helpers.cpp \
			http_routes.cpp Chronometer.cpp \
			WebServ_post_process.cpp WebServ_proc_header.cpp \
			WebServ_operator_out.cpp WebServ_ports.cpp WebServ.cpp \
			TemplatePage.cpp cgi.cpp WebServ_works.cpp WebServ_dispatches.cpp
SRCS1	=	main.cpp
HEAD	=	$(TRACKMAKEFILE) argval_ws.conf header.hpp \
			FileString.hpp DataFold.hpp StringTools.hpp ArgVal.hpp \
			datafold_type.hpp DataFold_defines.hpp bladefs.hpp WebServ.hpp \
			Chronometer.hpp TemplatePage.hpp
SHELL	=	/bin/sh
CC		=	$(MAKESILENT) c++ -std=c++98 -Wfatal-errors -DVERBOSE=$(VERBOSE) $(ENVS)
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

ws:			line $(NAME1)
all:		intro line $(NAME1) lynx siege clean lynx-clean siege-clean line
1:			ws
intro:		line
	@echo "Using CC CCFLAGS = $(CC) $(CCFLAGS)"
outro:		line
line:
	@echo "\n************************\n"
$(NAME1):	$(OBJS1) $(OBJS)
	$(CC) $(CCFLAGS) $(OBJS) $(OBJS1) -o $(NAME1)
	@./set_working_directory.sh
$(OBJS):	%.o : %.cpp $(HEAD)
	$(DOT)
	$(CC) $(CCFLAGS) -o $@ -c $<
$(OBJS1):	%.o : %.cpp
	$(DOT)
	$(CC) $(CCFLAGS) -o $@ -c $<
$(OBJS2):	%.o : %.cpp
	$(DOT)
	$(CC) $(CCFLAGS) -o $@ -c $<
clean:		lynx-clean siege-clean
	-@rm -f $(OBJS)
	-@rm -f $(OBJS1)
	-@rm -f $(OBJS2)
fclean:		clean lynx-fclean siege-fclean
	-@rm -f $(NAME1)
	@./unset_working_directory.sh
re:			fclean all
rt:			re t
v:			1
	$(VAL) ./$(NAME1) $(ARGS1)
vf:			1
	$(VAL) $(VALFLAG) ./$(NAME1) $(ARGS1)
test:		t
t:			1
	@echo "\n=======> Please run unit/general_tests.sh on another terminal <======="
	./$(NAME1) $(ARGS1)
g:			1
	gdb --args ./$(NAME1) $(ARGS1)
k1:
	-pkill $(NAME1)

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

####### ::::::::::: #######
####### :: siege :: #######
####### ::::::::::: #######

siege:
	@[ -f siege ] && exit 0 ||:
	@echo "\nBuilding siege standalone..."
	@-cd siege-standalone && \
	make -s > /dev/null 2> /dev/null
	@mv siege-standalone/src/siege .
siege-fclean:
	@cd siege-standalone && \
	make -s fclean
	@rm -f siege
siege-clean:
	@cd siege-standalone && \
	make -s clean
siege-re:
	cd siege-standalone && \
	make re

####### :::::::::: #######
