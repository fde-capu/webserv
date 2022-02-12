# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/02/12 11:38:34 by fde-capu          #+#    #+#              #
#    Updated: 2022/02/12 12:57:21 by fde-capu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SHELL	=	/bin/bash
DIRS	=	src
BIN		=	lynx lynx.cfg lynx.lss

all:	$(BIN)

$(BIN):	$(DIRS)
	cd src && \
	./configure --sysconfdir=$(PWD) && \
	make
	mv src/lynx .
	mv src/samples/lynx.lss .
	sed src/lynx.cfg -e "s/^STARTFILE:.*/STARTFILE:http:\/\/localhost\//" > ./lynx.cfg

$(DIRS):
	mkdir -p $(PWD)/src
	tar -zxvf lynx2.8.9rel.1.tar.gz -C src --strip 1

clean:
	rm -rf $(PWD)/src

fclean:	clean
	-rm $(BIN)

re: fclean all
