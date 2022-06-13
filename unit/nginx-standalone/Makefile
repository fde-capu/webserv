# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/02/07 20:55:24 by fde-capu          #+#    #+#              #
#    Updated: 2022/06/13 15:11:18 by fde-capu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SHELL	=	/bin/bash
BIN		=	$(PWD)/sandbox/sbin/nginx
DIRS	=	nginx-src pcre

all:	$(BIN)

$(BIN):
	cd nginx-src && \
	./configure --prefix=$(PWD)/sandbox --with-pcre=$(PWD)/pcre \
		--with-http_dav_module \
	&& \
	make && \
	make install \
	|| echo 'Run `make unpack` before making all.'

$(DIRS):
unpack: $(DIRS)
	mkdir -p $(PWD)/sandbox
	mkdir -p $(PWD)/pcre
	tar -xzvf pcre-8.45.tar.gz -C pcre --strip 1
	mkdir -p $(PWD)/nginx-src
	tar -xzvf nginx-1.20.2.tar.gz -C nginx-src --strip 1

clean:
	rm -rf $(PWD)/nginx-src

fclean: clean
	-pkill nginx
	rm -rf $(PWD)/sandbox
	rm -rf $(PWD)/pcre

re:	fclean all
