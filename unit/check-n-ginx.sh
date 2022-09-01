#!/bin/sh
# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    check-n-ginx.sh                                    :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fde-capu <fde-capu@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/09/01 13:37:36 by fde-capu          #+#    #+#              #
#    Updated: 2022/09/01 13:37:36 by fde-capu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

set -x

pkill nginx

{ set +x; } 2> /dev/null;
echo "\n";
echo "/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\\";
			ps aux | grep nginx | grep -v grep;
echo " ^^^ Should NOT see nginx proccess above. ^^^\n\n";
echo ;
netstat -tnl || ss -tnl
echo '^ No ports are open (besides default ones).'
echo ;
echo 'Run:\n\tnginx -tc ./nginx-as-webserv.conf\nthen:\n\tnginx -c ./nginx-as-webserv.conf';
echo 'Once nginx is running, check `ps aux` and `netstat -tnl`.';
echo 'Then follow general_tests.sh.';
echo ;
