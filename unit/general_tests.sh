#!/bin/sh

#`./webserv`
name_server="127.0.0.1";

# On 42SP Workspace, there are opened:

#	tcp 	0	0.0.0.0:5901	0.0.0.0:*	LISTEN	-	(VNC server)
#	tcp6	0	:::5901	:::*	LISTEN	-		
#	tcp 	0	0.0.0.0:2222	0.0.0.0:*	LISTEN	-	(SSH)
#	tcp6	0	:::2222	:::*	LISTEN	-		
#	tcp 	0	0.0.0.0:8080	0.0.0.0:*	LISTEN	-	(./login)		

# We will open the following ports:

#	Demonstration of implementations:
#	tcp 	0	0.0.0.0:3490	0.0.0.0:*	LISTEN	67/nginx: master pr	

#	Another server:port pointing to another folder:
#	tcp 	0	0.0.0.0:3491	0.0.0.0:*	LISTEN	67/nginx: master pr	

#	Lack of `location` gives forbidden all:
#	tcp 	0	0.0.0.0:3492	0.0.0.0:*	LISTEN	67/nginx: master pr	

#	Redirect 301 to :3490:
#	tcp 	0	0.0.0.0:3493	0.0.0.0:*	LISTEN	67/nginx: master pr	

#	To run the subject tests:
#	tcp 	0	0.0.0.0:4242	0.0.0.0:*	LISTEN	67/nginx: master pr	

# (helpers) #######################################################

MYSELF="$(realpath "$0")"
MYDIR="${MYSELF%/*}"

div()
{
	{ set +x; } 2> /dev/null
	echo "\n\n # # # # # #\n"
	set -x
}

divider()
{
	{ set +x; } 2> /dev/null
	echo "\n\n######### $1 #####################################"
	set -x
}

anounce()
{
	{ divider $1; } 2> /dev/null
	echo $2
	{ echo '-------------------------------------------------'; } 2> /dev/null
}

for i in 1 2 3 4 5 6 7 8 9 10
do
	{ divider "#"; } 2> /dev/null
done

## A ################################################################
#
{ anounce A \
\
	':3490 will demonstrate the implementations. \n
	It is bound to directory ./unit/confs/html.	\n
	Must receive 200 OK and some html body from index.html.' \
\
; } 2> /dev/null

if ! curl -v http://$name_server:3490; then
	 { anounce ERROR 'Make sure the server is running!'; } 2> /dev/null;
	 exit 1;
fi
#
## B ################################################################
#
#{ anounce B \
#\
#	'Accepts subdirectory calls. \n
#	-L tells curl to follow redirect. \n
#	nginx returns 301 on these cases, and client folows, \n
#	but webserv here is directly serving.' \
#\
#; } 2> /dev/null
#
#curl -vL http://$name_server:3490/somesub
#
## C ################################################################
#
#{ anounce C \
#\
#	"Subdirectory ending with '/' has the same effect. \n
#	This time, curl -L is not required." \
#\
#; } 2> /dev/null
#
#curl -v http://$name_server:3490/somesub/
#
## D ################################################################
#
#{ anounce D \
#\
#	'Send Host header as an existent server_name. \n
#	Rooted on ./unit/confs/html-custom-server-name.' \
#\
#; } 2> /dev/null
#
#curl -v http://$name_server:3490 -H 'Host: krazything'
#
## E ################################################################
#
#{ anounce E \
#\
#	'Unexistent servername defaults to :3490 first declaration.' \
#\
#; } 2> /dev/null
#
#curl -v http://$name_server:3490 -H 'Host: unexistent_servername'
#
## FA ###############################################################
#
#{ anounce FA \
#\
#	':3491, another port, another server, another folder: \n
#	root ./unit/confs/html-3491.' \
#\
#; } 2> /dev/null
#
#curl -v http://$name_server:3491
#
## FB ###############################################################
#
#{ anounce FB \
#\
#	':3491 accepts only GET. Testing POST is not allowed.' \
#\
#; } 2> /dev/null
#
#curl -v -X POST -F "file=@${MYDIR}/1MiB.noise" http://$name_server:3491
#
## FC ###############################################################
#
#{ anounce FC \
#\
#	':3491 accepts only GET. Testing DELETE is not allowed.' \
#\
#; } 2> /dev/null
#
#curl -v -X DELETE http://$name_server:3491
#
## G ################################################################
#
#{ anounce G \
#\
#	':3492 is solenly a `server { listen 3492; }`, \n
#	this is an open port, but forbidden serverside, \n
#	because it has no location, root, nor redirect. (403)' \
#\
#; } 2> /dev/null
#
#curl -v http://$name_server:3492
##
### H ################################################################
##
#{ anounce H \
#\
#	':3493 server redirects 301 to :3490. \n
#	- client redirecting:' \
#\
#; } 2> /dev/null
#
#curl -v http://$name_server:3493 -L
#
## I ################################################################
#
#{ anounce I \
#\
#	' - client NOT redirecting (gets 301):\n
#	(dumb test, this is client-side).' \
#\
#; } 2> /dev/null
#
#curl -v http://$name_server:3493
#
## J ################################################################
#
#{ anounce J \
#\
#	'Testing :4242 specifics. Will now use location. \n
#	GET on / should be ok.' \
#\
#; } 2> /dev/null
#
#curl -v http://$name_server:4242
#
## KA ###############################################################
#
#{ anounce KA \
#\
#	'POST :4242 at root (/) should fail, 405 not allowed.' \
#\
#; } 2> /dev/null
#
#curl -vL -X POST -F "file=@${MYDIR}/1MiB.noise" http://$name_server:4242 
#
### KB ###############################################################
#
#{ anounce KB \
#\
#	'PUT :4242 at root (/) also should fail. 405' \
#\
#; } 2> /dev/null
#
#curl -vL -X PUT -d faa=fee -d fii=foo http://$name_server:4242
#
### KC ###############################################################
#
#{ anounce KC \
#\
#	'DELETE :4242 at root (/) also should fail. 405' \
#\
#; } 2> /dev/null
#
#curl -vL -X DELETE http://$name_server:4242
#
### LA ###############################################################
#
#{ anounce LA \
#\
#	'/post_body rejects all but POST (2 tests). 405' \
#\
#; } 2> /dev/null
#
#curl -vD- -X GET http://$name_server:4242/post_body
#{ div; } 2> /dev/null
#curl -vD- -X DELETE http://$name_server:4242/post_body
#
# LB ###############################################################

#{ anounce LB \
#\
#	'- /post_body must answer anything to POST request with a \n
#	maxBody of 100. Three tests: 99B (200), 100B (200) and 101B. (405)' \
#\
#; } 2> /dev/null
#
#curl -X POST -vF "file=@${MYDIR}/99B.noise" http://$name_server:4242/post_body
#{ div; } 2> /dev/null
#curl -X POST -vF "file=@${MYDIR}/100B.noise" http://$name_server:4242/post_body
#{ div; } 2> /dev/null
#curl -X POST -vF "file=@${MYDIR}/101B.noise" http://$name_server:4242/post_body

#################################################################
#################################################################
#################################################################

#curl -v http://$name_server:4242/directory/
#curl -vLD- http://$name_server:4242/directory/youpi.bla

#curl -vLD- http://$name_server:3490/.php

#curl -v http://$name_server:4242/directory/unexistent_file

#curl http://$name_server:3490/test.php -v
#curl http://$name_server:3490/cgi-bin/test.php -v

#curl -vLD- http://127.0.0.1:4242/directory/oulalala
#curl -vLD- http://$name_server:4242/directory/Yeah

#curl -vLD- -X POST http://$name_server:4242/post_body -F 'file=./1M.noise'
#curl -vLD- -X POST http://$name_server:4242/directory/youpi.bla

#################################################################
