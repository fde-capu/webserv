# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/02/12 11:38:34 by fde-capu          #+#    #+#              #
#    Updated: 2022/02/12 17:30:10 by fde-capu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SHELL	=	/bin/bash
DIRS	=	$(PWD)/src
BIN		=	$(PWD)/lynx $(PWD)/lynx.cfg $(PWD)/lynx.lss

all:	$(BIN)

$(BIN):
	mkdir -p $(PWD)/src
	tar -zxvf lynx2.8.9rel.1.tar.gz -C src --strip 1
	cd src && \
	./configure --sysconfdir=$(PWD) && \
	make
	mv src/lynx .
	mv src/samples/lynx.lss .
	sed src/lynx.cfg -e "s/^STARTFILE:.*/STARTFILE:http:\/\/localhost:3490\//" > ./lynx.cfg
	-source source-me-after-install-rc

clean:
	rm -rf $(PWD)/src

fclean:	clean
	-rm $(BIN)

re: fclean all
