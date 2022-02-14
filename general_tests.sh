#!/bin/bash

# On 42SP Workspace, there are opened:

#	tcp 	0	0.0.0.0:5901	0.0.0.0:*	LISTEN	-	(VNC server)
#	tcp6	0	:::5901	:::*	LISTEN	-		
#	tcp 	0	0.0.0.0:2222	0.0.0.0:*	LISTEN	-	(Invalid SSH)
#	tcp6	0	:::2222	:::*	LISTEN	-		
#	tcp 	0	0.0.0.0:8080	0.0.0.0:*	LISTEN	-	(./login)		

# We will open:

#	tcp 	0	0.0.0.0:4242	0.0.0.0:*	LISTEN	67/nginx: master pr	
#	To run the subject tests.

#	tcp 	0	0.0.0.0:3490	0.0.0.0:*	LISTEN	67/nginx: master pr	
#	Demonstration of implementations.

#	tcp 	0	0.0.0.0:3491	0.0.0.0:*	LISTEN	67/nginx: master pr	
#	Another server:port pointing to another folder.

#	tcp 	0	0.0.0.0:3492	0.0.0.0:*	LISTEN	67/nginx: master pr	
#	Lack of `location` gives forbidden all.

#	tcp 	0	0.0.0.0:3493	0.0.0.0:*	LISTEN	67/nginx: master pr	
#	Redirect 301 to :3490.

divider()
{
	{ set +x; } 2> /dev/null
	echo -e '\n\n--------------------------\n\n'
	set -x
}

anounce()
{
	{ divider; } 2> /dev/null
	echo -e $1
	echo -e '\n'
}

{ anounce \
\
	'Hello, have fun!' \
\
; } 2> /dev/null

{ divider; } 2> /dev/null
{ divider; } 2> /dev/null
{ divider; } 2> /dev/null

#################################################################

{ anounce \
\
	':3490 will demonstrate the implementations.' \
\
; } 2> /dev/null

curl -vD- http://localhost:3490

#################################################################

{ anounce \
\
	'Accepts subdirectory calls.' \
\
; } 2> /dev/null

curl -vD- http://localhost:3490/somesub/

#################################################################

{ anounce \
\
	'Send Host header as another server_name.' \
\
; } 2> /dev/null

curl -vD- http://localhost:3490 -H 'Host: krazything'

#################################################################

{ anounce \
\
	'This call must default to :3490 (first conf declaration).' \
\
; } 2> /dev/null

curl -vD- http://localhost:3490 -H 'Host: unexistent_servername'

#################################################################

{ anounce \
\
	':3491, another port, another server, another folder:' \
\
; } 2> /dev/null

curl -vD- http://localhost:3491

#################################################################

{ anounce \
\
	':3492, no location at all.' \
\
; } 2> /dev/null

curl -vD- http://localhost:3492

#################################################################

{ anounce \
\
	':3493 redirects 301 to :3490. \n 
	  - client redirecting:' \
\
; } 2> /dev/null

curl -vD- http://localhost:3493 -L

#################################################################

{ anounce \
\
	' - client NOT redirecting:' \
\
; } 2> /dev/null

curl -vD- http://localhost:3493

#################################################################

{ anounce \
\
	'Specific test for debug:' \
\
; } 2> /dev/null

curl -vD- http://localhost:4242
echo '----------------------'
curl -vD- http://localhost:4242/directory/
