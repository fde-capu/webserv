#!/bin/sh
set -x

pkill nginx

set +x;
echo "\n";
echo "/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\\";

set -x;
			ps aux | grep nginx | grep -v grep;
set +x;

echo " ^^^ Should NOT see nginx proccess above. ^^^\n\n";
echo 'Run:\n\tnginx -tc ./nginx-as-webserv.conf\nthen:\n\tnginx -c ./nginx-as-webserv.conf';
echo 'Once nginx is running, check with \`ps aux\` and \`netstat -tnlp\`.';
echo 'Then follow general_tests.sh.';
