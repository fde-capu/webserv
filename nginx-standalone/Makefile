# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/02/07 20:55:24 by fde-capu          #+#    #+#              #
#    Updated: 2022/02/07 20:56:09 by fde-capu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

all:
	mkdir -p $(PWD)/sandbox
	mkdir -p $(PWD)/pcre
	mkdir -p $(PWD)/nginx-src
	tar -xzvf pcre-8.45.tar.gz -C pcre --strip 1
	tar -xzvf nginx-1.20.2.tar.gz -C nginx-src --strip 1
	
	cd nginx-src && \
	./configure --prefix=$(PWD)/sandbox --with-pcre=$(PWD)/pcre && \
	make && \
	make install && \
	sed -i 's/80;/3491;/' $(PWD)/sandbox/conf/nginx.conf
	alias nginx=$(PWD)/sandbox/sbin/nginx

clean:
	-pkill nginx
	rm -rf $(PWD)/sandbox
	rm -rf $(PWD)/pcre
	rm -rf $(PWD)/nginx-src

re:	clean all

fclean: clean
