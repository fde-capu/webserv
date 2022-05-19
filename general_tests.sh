#!/bin/sh

name_server="127.0.0.1";

# On 42SP Workspace, there are opened:

`netstat -tnl`
#	tcp 	0	0.0.0.0:5901	0.0.0.0:*	LISTEN	-	(VNC server)
#	tcp6	0	:::5901	:::*	LISTEN	-		
#	tcp 	0	0.0.0.0:2222	0.0.0.0:*	LISTEN	-	(Invalid SSH)
#	tcp6	0	:::2222	:::*	LISTEN	-		
#	tcp 	0	0.0.0.0:8080	0.0.0.0:*	LISTEN	-	(./login)		

`./webserv`
# We will open (as webserv-default.conf describes):

#	To run the subject tests:
#	tcp 	0	0.0.0.0:4242	0.0.0.0:*	LISTEN	67/nginx: master pr	

#	Demonstration of implementations:
#	tcp 	0	0.0.0.0:3490	0.0.0.0:*	LISTEN	67/nginx: master pr	

#	Another server:port pointing to another folder:
#	tcp 	0	0.0.0.0:3491	0.0.0.0:*	LISTEN	67/nginx: master pr	

#	Lack of `location` gives forbidden all:
#	tcp 	0	0.0.0.0:3492	0.0.0.0:*	LISTEN	67/nginx: master pr	

#	Redirect 301 to :3490:
#	tcp 	0	0.0.0.0:3493	0.0.0.0:*	LISTEN	67/nginx: master pr	

divider()
{
	{ set +x; } 2> /dev/null
	echo '\n\n--------------------------\n\n'
	set -x
}

anounce()
{
	{ divider; } 2> /dev/null
	echo $1
	{ divider; } 2> /dev/null
}

{ anounce \
\
	'Hello, have fun!' \
\
; } 2> /dev/null

{ divider; } 2> /dev/null
{ divider; } 2> /dev/null
{ divider; } 2> /dev/null
{ divider; } 2> /dev/null
{ divider; } 2> /dev/null
{ divider; } 2> /dev/null

# A ################################################################

{ anounce \
\
	':3490 will demonstrate the implementations.' \
\
; } 2> /dev/null

curl -vD- http://$name_server:3490

# B ################################################################

{ anounce \
\
	'Accepts subdirectory calls.' \
\
; } 2> /dev/null

curl -vD- http://$name_server:3490/somesub

# C ################################################################

{ anounce \
\
	"Subdirectory ending with '/'." \
\
; } 2> /dev/null

curl -vD- http://$name_server:3490/somesub/

# D ################################################################

{ anounce \
\
	'Send Host header as another server_name (existent).' \
\
; } 2> /dev/null

curl -vD- http://$name_server:3490 -H 'Host: krazything'

# E ################################################################

{ anounce \
\
	'Unexistent servername must default to :3490 (first conf declaration).' \
\
; } 2> /dev/null

curl -vD- http://$name_server:3490 -H 'Host: unexistent_servername'

# F ################################################################

{ anounce \
\
	':3491, another port, another server, another folder:' \
\
; } 2> /dev/null

curl -vD- http://$name_server:3491

# G ################################################################

{ anounce \
\
	':3492, no location at all.' \
\
; } 2> /dev/null

curl -vD- http://$name_server:3492

# H ################################################################

{ anounce \
\
	':3493 redirects 301 to :3490. \n 	  - client redirecting:' \
\
; } 2> /dev/null

curl -vD- http://$name_server:3493 -L

# I ################################################################

{ anounce \
\
	' - client NOT redirecting:' \
\
; } 2> /dev/null

curl -vD- http://$name_server:3493

# J ################################################################

{ divider; } 2> /dev/null

#curl -vD- http://$name_server:4242/directory/
#curl -vLD- http://$name_server:4242/directory/youpi.bla

#curl -vLD- http://$name_server:3490/.php

#curl -vD- http://$name_server:4242/directory/unexistent_file

#curl http://$name_server:3490/test.php -vD-
#curl http://$name_server:3490/cgi-bin/test.php -vD-

#curl -vLD- http://127.0.0.1:4242/directory/oulalala
#curl -vLD- http://$name_server:4242/directory/Yeah

#curl -vLD- -X POST http://$name_server:4242/post_body -F 'file=./1M.noise'
#curl -vLD- -X POST http://$name_server:4242/directory/youpi.bla

#################################################################
