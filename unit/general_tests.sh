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

#	To run the subject tests 42 ubuntu_tester and ubuntu_cgi_tester:
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

{ ${MYDIR}/clean_uploads.sh; }

{ set +x; } 2> /dev/null
if false; then
	echo 'foo';


#################################################################### Begin



## Basic_1 ################################################################

{ anounce Basic_1 \
\
	':3490 will demonstrate the implementations. \n
	It is bound to directory ./unit/confs/html.	\n
	Must receive 200 OK and some html body from html/index.html.' \
\
; } 2> /dev/null

if ! curl -v http://$name_server:3490; then
	 { anounce ERROR 'Make sure the server is running!'; } 2> /dev/null;
	 exit 1;
fi

## Basic_2 ################################################################

{ anounce Basic_2 \
\
	'If server_name is unexistent, defaults to previous: 200' \
\
; } 2> /dev/null

curl -v http://$name_server:3490 -H 'Host: wtf_server'

## Basic_3 ################################################################

{ anounce Basic_3 \
\
	'If server_name is existent on another root: 200' \
\
; } 2> /dev/null

curl -v http://$name_server:3490 -H 'Host: krazything'

## Basic_4 ################################################################

{ anounce Basic_4 \
\
	'Existent server_name without root definition. 403' \
\
; } 2> /dev/null

curl -v http://$name_server:3490 -H 'Host: rootless'

## Basic_5 ################################################################

fi # > > > > > > > > > > > > > > > > > > > > > > > > > > > Jump line!
{ anounce Basic_5 \
\
	'POST test. Within limits of client_max_body_size:' \
\
; } 2> /dev/null

curl -D- --trace-ascii log -F "file=@${MYDIR}/99B.words" \
	http://$name_server:3490 && cat log && rm log
exit; # < < < < < < < < < < < < < < < < < < < < < < < < < < End line!

## Basic_6 ################################################################

{ anounce Basic_6 \
\
	'POST test. 99B again, noise this time.' \
\
; } 2> /dev/null

head -c 99 /dev/urandom > ${MYDIR}/file.noise
curl -D- --trace-ascii log -X POST -F "file=@${MYDIR}/file.noise" \
	http://$name_server:3490 && cat log && rm log
rm ${MYDIR}/file.noise


# B ################################################################

{ anounce B \
\
	'Accepts subdirectory calls. \n
	-L tells curl to follow redirect. \n
	nginx returns 301 on these cases, and client folows, \n
	but webserv here is directly serving.' \
\
; } 2> /dev/null

curl -vL http://$name_server:3490/somesub

# C ################################################################

{ anounce C \
\
	"Subdirectory ending with '/' has the same effect. \n
	This time, curl -L is not required." \
\
; } 2> /dev/null

curl -v http://$name_server:3490/somesub/

# D ################################################################

# E ################################################################

{ anounce E \
\
	'Unexistent servername defaults to :3490 first declaration.' \
\
; } 2> /dev/null

curl -v http://$name_server:3490 -H 'Host: unexistent_servername'

# FA ###############################################################

{ anounce FA \
\
	':3491, another port, another server, another folder: \n
	root ./unit/confs/html-3491.' \
\
; } 2> /dev/null

curl -v http://$name_server:3491

# FB ###############################################################

{ anounce FB \
\
	':3491 accepts only GET. Testing POST is not allowed. 405' \
\
; } 2> /dev/null

curl -v -X POST -F "file=@${MYDIR}/1MiB.noise" http://$name_server:3491

# FC ###############################################################

{ anounce FC \
\
	':3491 accepts only GET. Testing DELETE is not allowed. 405' \
\
; } 2> /dev/null

curl -v -X DELETE http://$name_server:3491

# G ################################################################

{ anounce G \
\
	':3492 is solenly a `server { listen 3492; }`, \n
	this is an open port, but forbidden serverside, \n
	because it has no location, root, nor redirect. (403)' \
\
; } 2> /dev/null

curl -v http://$name_server:3492
#
## H ################################################################
#
{ anounce H \
\
	':3493 server redirects 301 to :3490. \n
	- client redirecting:' \
\
; } 2> /dev/null

curl -v http://$name_server:3493 -L

# I ################################################################

{ anounce I \
\
	' - client NOT redirecting (gets 301):\n
	(dumb test, this is client-side).' \
\
; } 2> /dev/null

curl -v http://$name_server:3493

# J ################################################################

{ anounce J \
\
	'Testing :4242 specifics. Will now use location. \n
	GET on / should be ok.' \
\
; } 2> /dev/null

curl -v http://$name_server:4242

# KA ###############################################################

{ anounce KA \
\
	'POST :4242 at root (/) should fail, 405 not allowed.' \
\
; } 2> /dev/null

curl -vL -X POST -F "file=@${MYDIR}/1MiB.noise" http://$name_server:4242 

## KB ###############################################################

{ anounce KB \
\
	'PUT :4242 at root (/) also should fail. 405' \
\
; } 2> /dev/null

curl -vL -X PUT -d faa=fee -d fii=foo http://$name_server:4242

## KC ###############################################################

{ anounce KC \
\
	'DELETE :4242 at root (/) also should fail. 405' \
\
; } 2> /dev/null

curl -vL -X DELETE http://$name_server:4242

## LA ###############################################################

{ anounce LA \
\
	'/post_body rejects all but POST (2 tests). 405' \
\
; } 2> /dev/null

curl -vD- -X GET http://$name_server:4242/post_body
{ div; } 2> /dev/null
curl -vD- -X DELETE http://$name_server:4242/post_body

## LB ###############################################################

{ anounce LB_1st \
\
	'- /post_body must answer anything to POST request with a \n
	maxBody of 100. Many tests.\n
	- 1st) Post 99B.words into html4242/uploads (as set in config).' \
\
; } 2> /dev/null

curl -D- --trace-ascii log -X POST -F "file=@${MYDIR}/99B.words" \
	http://$name_server:4242/post_body && cat log && rm log


{ anounce LB_2nd \
\
	'2nd) The same, using noise file.' \
\
; } 2> /dev/null

curl -D- --trace-ascii log -X POST -F "file=@${MYDIR}/99B.noise" \
	http://$name_server:4242/post_body && cat log && rm log

{ anounce LB_3rd \
\
	'3rd) Same, with 100B.noise. Max is at 100B, this passes.' \
\
; } 2> /dev/null

curl -X POST -vF "file=@${MYDIR}/100B.noise" http://$name_server:4242/post_body

{ anounce LB_4th \
\
	'4th) 101B.noise should not pass because of max_size. 413' \
\
; } 2> /dev/null

 curl -X POST -vF "file=@${MYDIR}/101B.noise" http://$name_server:4242/post_body

## Large Uploads ################################################################

{ anounce Large_Uploads_1 \
\
	'Testing 1MiB.noise. This should be reject by 413 because exceeds max_size.' \
\
; } 2> /dev/null

curl -X POST -vF "file=@${MYDIR}/1MiB.noise" http://$name_server:4242/post_body

{ anounce Large_Uploads_2 \
\
	'Not large, but shows "uri /large_upload" is working. 202' \
\
; } 2> /dev/null

 curl -X POST -vF "file=@${MYDIR}/100B.noise" http://$name_server:4242/large_upload


{ anounce Large_Uploads_3 \
\
	'Now POSTing 1MiB.noise.\n
	This shall NOT be accepted, because curl will expect 100-continue,\n
	but webserv must always close the connection. Chose 424 for answer.' \
\
; } 2> /dev/null

 curl -X POST -vF "file=@${MYDIR}/1MiB.noise" http://$name_server:4242/large_upload


{ anounce Large_Uploads_4 \
\
	'Again 1MiB.noise, this time sending the file right away.\n
	This SHALL BE accepted and saved.' \
\
; } 2> /dev/null

 curl -X POST -vF "file=@${MYDIR}/1MiB.noise" -H "Expect:" http://$name_server:4242/large_upload

## Stress ################################################################

stress_count=300;

{ anounce Stress \
\
	"Stress testing $stress_count calls. Wait for it.\n
	This can be changed on general_tests.sh:stress_count" \
\
; } 2> /dev/null

set +x;

rm -f stress_out
i=1;
while [ "$i" -le "$stress_count" ]; do
	echo -n "\r $i";
	curl -sv -H "Keep-Alive: 60" -H "Connection: keep-alive" http://localhost:3491 2>> stress_out 1> /dev/null
	i=$(( i + 1 ))
done
echo -n "\rCount of '200 OK' responses must be $stress_count, and it is: ";
cat stress_out | grep HTTP | grep "200 OK" | wc -l;
rm -f stress_out
set -x;

## Double_call ################################################################

{ anounce Double_call \
\
	'Double test: make two simultaneous calls. Not mandatory. \n
	(not implemented, though taken care not to crash).' \
\
; } 2> /dev/null

curl -X POST -F "file=@${MYDIR}/99B.words" \
	http://$name_server:4242/post_body & \
curl -X POST -F "file=@${MYDIR}/99B.words" \
	http://$name_server:4242/post_body


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
