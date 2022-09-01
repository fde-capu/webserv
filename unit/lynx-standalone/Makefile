# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/02/12 11:38:34 by fde-capu          #+#    #+#              #
#    Updated: 2022/09/01 13:31:53 by fde-capu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SHELL	=	/bin/bash
DIRS	=	$(PWD)/src
BIN		=	$(PWD)/lynx $(PWD)/lynx.cfg $(PWD)/lynx.lss

all:	$(BIN)

$(BIN):
	@[ ! -d $(PWD)/src ] && make -s unpack || :
	echo "Lynx configure..."
	cd src && \
	./configure --sysconfdir=$(PWD) && \
	make -s
	mv src/lynx .
	mv src/samples/lynx.lss .
	sed src/lynx.cfg -e "s/^STARTFILE:.*/STARTFILE:http:\/\/localhost:3490\//" > ./lynx.cfg

unpack:
	echo "Unpacking lynx..."
	mkdir -p $(PWD)/src
	tar -zxf lynx2.8.9rel.1.tar.gz -C src --strip 1

clean:
	@rm -rf $(PWD)/src

fclean:	clean
	@rm -f $(BIN)

re: fclean all
