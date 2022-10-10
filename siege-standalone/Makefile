# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/10/11 00:09:16 by fde-capu          #+#    #+#              #
#    Updated: 2022/10/11 00:55:15 by fde-capu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SHELL	=	/bin/bash
DIRS	=	$(PWD)/src
BIN		=	$(PWD)/siege

all:	$(BIN)

$(BIN):
	@[ ! -d $(PWD)/src ] && make -s unpack ||:
	echo "Siege configure..."
	cd src && ./configure --bindir=$(PWD)/src
	cd src && make -s
	cd src && make install

unpack:
	echo "Unpacking siege..."
	mkdir -p $(PWD)/src
	tar -zxf siege-latest.tar.gz -C src --strip 1

clean:
	@rm -rf $(PWD)/src

fclean:	clean
	@rm -f $(BIN)

re: fclean all
