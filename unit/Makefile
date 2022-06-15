# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/02/10 01:00:04 by fde-capu          #+#    #+#              #
#    Updated: 2022/06/15 16:33:59 by fde-capu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


####### :::::::::: #######
####### :: unit :: #######
####### :::::::::: #######

SERVER	=	127.0.0.1
FTPORT	=	4242

WEBSERV_C =	webserv-unit.conf
NGINX_C	=	nginx-as-webserv.conf

SRCS	=	
HEAD	=	Makefile
SHELL	=	/bin/bash
CC		=	clang++ -std=c++98
CCFLAGS	=	-Wall -Werror -Wextra -g
VAL		=	valgrind
VALFLAG	=	--tool=memcheck \
			--leak-check=full \
			--show-leak-kinds=all \
			--track-origins=yes \
			--show-reachable=yes
NGINX	=	$(PWD)/nginx-standalone/sandbox/sbin/nginx
RUNCGI	=	@echo "Please \`make ftcgi\` on another terminal.";
SS		=	netstat -tnlp || ss -tnlp
MAKEFLAGS =	--no-print-directory

all:		nginx lynx
clean:		nginx-clean lynx-clean
fclean:		clean nginx-fclean lynx-fclean
re:			fclean all
t:			nginx-t


####### ::::::::::::: #######
####### :: webserv :: #######
####### ::::::::::::: #######

WSNOTDAEMON	=	echo '==========> Webserv does not run as daemon. On another terminal, please run `make webserv-gt`. <=========';

webserv-build:
	@echo 'webserv: '
	-@cd .. && \
	make
webserv-up:	webserv
webserv:	webserv-k webserv-build $(WEBSERV_C) nginx-k
	@$(WSNOTDAEMON)
	../webserv $(PWD)/$(WEBSERV_C)
webserv-r:	webserv
webserv-k:
	-pkill webserv
	$(SS)
$(WEBSERV_C): webserv-r
webserv-t:	webserv
webserv-gt:
	../general_tests.sh
webserv-rt:	webserv-r webserv-t
webserv-tc:	$(WEBSERV_C)
	@echo 'Test without running (like -t option on nginx) not implemented.';
webserv-clean:
	cd .. && \
	make clean
webserv-fclean:
	cd .. && \
	make fclean
webserv-re:
	cd .. && \
	make re
webserv-relog:
	@echo 'webserv log system not implemented.';
webserv-catlog:
	@echo 'webserv log system not implemented.';
webserv-tlog: webserv-relog webserv-t webserv-catlog
wst:	webserv-t


####### ::::::::::: #######
####### :: nginx :: #######
####### ::::::::::: #######

NGXUNPCKME	=	echo '=========> You need to `make nginx-pre-build` before actually building. (Do it if not already.) <=========='
NGXSRCME	=	echo '=========> Please run: `source ./source-me-after-install-rc`. <============'

$(NGINX):
nginx-pre-build: $(NGINX)
	-@cd nginx-standalone && \
	make unpack
nginx-build:
	@$(NGXUNPCKME)
	@echo 'nginx: '
	-@cd nginx-standalone && \
	make
nginx-up:	nginx
nginx:		nginx-k nginx-build $(NGINX_C) nginx-tc webserv-k
	$(NGINX) -c $(NGINX_C)
	$(SS)
nginx-k:
	-pkill nginx
	$(SS)
$(NGINX_C):	nginx-r
nginx-t:	nginx
	../general_tests.sh
nginx-r:	nginx
nginx-rt:	nginx-r nginx-t
nginx-tc:	$(NGINX_C)
	@-$(NGINX) -c $(NGINX_C) -t || $(NGXSRCME)
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


####### :::::::::: #######
####### :: lynx :: #######
####### :::::::::: #######

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


####### :::::::: #######
####### :: ft :: #######
####### :::::::: #######

ft: nginx use_ws_cgi
	$(RUNCGI)
	./ubuntu_tester http://$(SERVER):$(FTPORT)
ftcgi: use_ws_cgi nginx
	export REQUEST_METHOD=POST && \
	export SERVER_PROTOCOL="HTTP/1.1" && \
	export PATH_INFO="/home/coder/webserv/unit/" && \
	cd .. && ./cgi_webserv /home/coder/webserv/unit/ubuntu_cgi_tester 9000
ftcgig: use_ws_cgi
	export REQUEST_METHOD=POST && \
	export SERVER_PROTOCOL="HTTP/1.1" && \
	export PATH_INFO="/home/coder/webserv/unit/" && \
	cd .. && gdb --args ./cgi_webserv /home/coder/webserv/unit/ubuntu_cgi_tester 9000
xcgi: use_ws_cgi
	export REQUEST_METHOD=POST && \
	export SERVER_PROTOCOL="HTTP/1.1" && \
	export PATH_INFO="/home/coder/webserv/unit/" && \
	cd .. && ./cgi_webserv /home/coder/webserv/unit/ubuntu_cgi_tester 4242
dumbcgi: use_ws_cgi
	export REQUEST_METHOD=POST && \
	export SERVER_PROTOCOL="HTTP/1.1" && \
	export PATH_INFO="/home/coder/webserv/unit/" && \
	cd .. && ./cgi_webserv /home/coder/webserv/unit/dumb_cgi.sh 9000
use_ws_cgi:
	cd .. && make cgi
