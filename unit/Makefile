# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/02/10 01:00:04 by fde-capu          #+#    #+#              #
#    Updated: 2022/06/08 16:27:32 by fde-capu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SERVER	=	127.0.0.1
FTPORT	=	4242
NAME	=	webserv
NAME_C	=	
NGINX_C	=	nginx-as-webserv.conf
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
SRCME	=	@echo 'Please run: `source ./source-me-after-install-rc`.';
MAKEFLAGS =	--no-print-directory
all:		nginx-build lynx
	$(SRCME)
$(NAME):	$(OBJS)
	$(CC) $(CCFLAGS) $(OBJS) -o $(NAME)
$(OBJS):	%.o : %.cpp $(HEAD)
	$(CC) $(CCFLAGS) -o $@ -c $<
clean:		nginx-clean lynx-clean
	-rm -f $(OBJS)
fclean:		clean nginx-fclean lynx-fclean
	-rm -f $(NAME)
re:			fclean all

nginx-build:
	@echo 'nginx: '
	-@cd nginx-standalone && \
	make
nginx:		kill nginx-build $(NGINX_C)
	cd confs && $(NGINX) -c $(NGINX_C)
	netstat -tnl || ss -tnl
kill:
	-pkill nginx
	netstat -tnl || ss -tnl
$(NGINX_C):	nginx-r
t:			nginx-t
nginx-t:	nginx
	../general_tests.sh
nginx-r:	nginx
nginx-rt:	nginx-r nginx-t
nginx-tc:	$(NGINX_C)
	$(NGINX) -c $(NGINX_C) -t
nginx-clean:
	cd nginx-standalone && \
	make clean
nginx-fclean:
	cd nginx-standalone && \
	make fclean
nginx-re:
	cd nginx-standalone && \
	make re
nginx-relog:
	cd nginx-standalone/sandbox/logs && \
	echo '' > error.log && \
	echo '' > access.log
nginx-catlog:
	cd nginx-standalone/sandbox/logs && \
	cat access.log && cat error.log
nginx-tlog: nginx-relog nginx-t nginx-catlog

lynx:
	@echo 'lynx: '
	@-cd lynx-standalone && \
	make
lynx-fclean:
	cd lynx-standalone && \
	make fclean
lynx-clean:
	cd lynx-standalone && \
	make clean
lynx-re:
	cd lynx-standalone && \
	make re
lynx-nginx: lynx nginx
lynx-nginx-t: lynx nginx-t
g:	nginx-log

ft: nginx ws_and_cgi
	@echo "Please run \`make ftcgi\` on another terminal.";
	@echo "";
	./ubuntu_tester http://$(SERVER):$(FTPORT)
ftcgi: ws_and_cgi nginx
	export REQUEST_METHOD=POST && \
	export SERVER_PROTOCOL="HTTP/1.1" && \
	export PATH_INFO="/home/coder/webserv/unit/" && \
	cd .. && ./cgi_webserv /home/coder/webserv/unit/ubuntu_cgi_tester 9000
ftcgig: ws_and_cgi
	export REQUEST_METHOD=POST && \
	export SERVER_PROTOCOL="HTTP/1.1" && \
	export PATH_INFO="/home/coder/webserv/unit/" && \
	cd .. && gdb --args ./cgi_webserv /home/coder/webserv/unit/ubuntu_cgi_tester 9000
xcgi: ws_and_cgi
	export REQUEST_METHOD=POST && \
	export SERVER_PROTOCOL="HTTP/1.1" && \
	export PATH_INFO="/home/coder/webserv/unit/" && \
	cd .. && ./cgi_webserv /home/coder/webserv/unit/ubuntu_cgi_tester 4242
dumbcgi: ws_and_cgi
	export REQUEST_METHOD=POST && \
	export SERVER_PROTOCOL="HTTP/1.1" && \
	export PATH_INFO="/home/coder/webserv/unit/" && \
	cd .. && ./cgi_webserv /home/coder/webserv/unit/dumb_cgi.sh 9000
ws_and_cgi:
	cd .. && make
