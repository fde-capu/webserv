#!/bin/sh

stress_count=30;

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
ok_count=0;
ko_count=0;
tot_count=0;

enterkey()
{
	{ set +x; } 2> /dev/null
	echo -n "\t\t\t\t\t--> Next ";
	read anything;
	set -x
}

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
	enterkey;
	{ divider $1; } 2> /dev/null
	echo $2
	{ echo '-------------------------------------------------'; } 2> /dev/null
	{ set +x; } 2> /dev/null
}

getcode()
{
	eval "$1 -o /dev/null -s -w '%{http_code}'";
}

getbodyandcode()
{
	eval "$1 -s -w '%{http_code}' -v";
}

unittest()
{
	if [ "$noise" != "" ] ; then
		head -c $noise /dev/urandom > ${MYDIR}/file.noise
	fi;
	
	fullcmd="set -x; $cmd -sSvw '%{http_code}'";
	if [ "$file" = "" ] ; then
		fullcmd="$fullcmd -o tmp_response";
		tfile="$code";
	fi
	out=`eval "$fullcmd"`;

	if [ "$out" = "000" ]; then
		{ anounce ERROR 'Make sure the server is running!'; } 2> /dev/null;
		exit 1;
	fi

	if [ "$file" = "" ] ; then
		cat tmp_response;
		echo -n " ";
		rm tmp_response;
	fi
	echo $out;

	if [ "$file" != "" ] ; then
		tfile=`cat $file`;
		tfile="$tfile\n$code"
	fi;
	colorprint "$1" "$out" "$tfile"

	if [ "$noise" != "" ] ; then
		if [ "$file" != "" ] ; then
			colorprint "Noise" "`cat ${MYDIR}/file.noise`" "`cat $file`";
		fi
		rm ${MYDIR}/file.noise;
		if [ "$file" != "" ] ; then
			ls -l $file;
			cat $file;
		fi
	fi

	cmd="";
	code="";
	file="";
	noise="";
}

finish()
{
	{ anounce FINISHED \
	\
		'..........now........\n
		\t......the.....\n\t\t....results......' \
	\
	; } 2> /dev/null
	echo " Total:\t\t $tot_count";
	echo "\033[0;32m [ OK ] \t $ok_count \033[0;37m";
	if [ "$ko_count" = "0" ] ; then
		echo "\033[0;32m Nice! \033[0;37m\n";
	else
		echo "\033[0;31m [ KO ] \t $ko_count \t:(\033[0;37m";
	fi;
	exit 0;
}

colorprint()
{
	a=`echo "$2" | hd`;
	b=`echo "$3" | hd`;
	echo -n "$1 ";
	if [ "$a" = "$b" ] ; then
		echo "\033[0;32m [ OK ]\033[0;37m";
		ok_count=$((ok_count+1));
	else
		echo "\033[0;31m [ KO ] \033[0;37m";
		ko_count=$((ko_count+1));
	fi
	tot_count=$((tot_count+1));
}

for i in 1 2 3 4 5 6 7 8 9 10
do
	{ divider "#"; } 2> /dev/null
done

{ ${MYDIR}/clean_uploads.sh; }

{ set +x; } 2> /dev/null
if false; then
	echo 'Stablish Jump and End lines.';

#################################################################### Begin

fi # > > > > > > > > > > > > > > > > > > > > > > > > > > > Jump line!

## Basic_1 ################################################################

{ anounce Basic_1 \
\
	':3490 will demonstrate the implementations. \n
	It is bound to directory ./unit/confs/html.	\n
	Must receive 200 OK and some html body from html/index.html.' \
\
; } 2> /dev/null

cmd="curl http://$name_server:3490";
file="$MYDIR/confs/html/index.htm";
code="200";
unittest "Basic 1";

## Basic_2 ################################################################

{ anounce Basic_2 \
\
	'If server_name is unexistent, defaults to previous: 200' \
\
; } 2> /dev/null

cmd="curl http://$name_server:3490 -H 'Host: wtf_server'";
file="$MYDIR/confs/html/index.htm";
code="200";
unittest "Basic 2";

## Basic_3 ################################################################


{ anounce Basic_3 \
\
	'If server_name is existent on another root: 200' \
\
; } 2> /dev/null

cmd="curl http://$name_server:3490 -H 'Host: krazything'";
file="$MYDIR/confs/html-custom-server-name/index.html";
code="200";
unittest "Basic 3";

## Basic_4 ################################################################


{ anounce Basic_4 \
\
	'Existent server_name without root definition. 403' \
\
; } 2> /dev/null

cmd="curl http://$name_server:3490 -H 'Host: rootless'";
code="403";
unittest "Basic 4";

# Basic_5 ################################################################

{ anounce Basic_5 \
\
	'Accepts subdirectory calls. \n
	-L tells curl to follow redirect. \n
	nginx returns 301 on these cases, and client folows, \n
	but webserv here is directly serving.' \
\
; } 2> /dev/null

cmd="curl -vL http://$name_server:3490/somesub"
code="200"
file="$MYDIR/confs/html/somesub/index.htm";
unittest "Basic 5";

# Basic_6 ################################################################


{ anounce Basic_6 \
\
	"Subdirectory ending with '/' has the same effect. \n
	This time, curl -L is not required." \
\
; } 2> /dev/null

cmd="curl -vL http://$name_server:3490/somesub/"
code="200"
file="$MYDIR/confs/html/somesub/index.htm";
unittest "Basic 6";

# Basic_7 ################################################################


{ anounce Basic_7 \
\
	'Unexistent servername defaults to :3490 first declaration.' \
\
; } 2> /dev/null

cmd="curl http://$name_server:3490 -H 'Host: unexistent_servername'";
file="$MYDIR/confs/html/index.htm";
code="200";
unittest "Basic 7";

# Basic_8 ################################################################

{ anounce Basic_8 \
\
	':3491, another port, another server, another folder: \n
	root ./unit/confs/html-3491.' \
\
; } 2> /dev/null

cmd="curl http://$name_server:3491"
file="$MYDIR/confs/html-3491/index.html";
code="200";
unittest "Basic 8";

##################################################################



{ anounce Not_allowed_1 \
\
	':3491 accepts only GET. Testing POST is not allowed. 405' \
\
; } 2> /dev/null

cmd="curl -F \"file=@${MYDIR}/file.noise\" http://$name_server:3491";
code="405";
noise="1MiB";

unittest "Not allowed 1";

##################################################################

{ anounce Not_allowed_2 \
\
	':3491 accepts only GET. Testing DELETE is not allowed. 405' \
\
; } 2> /dev/null

cmd="curl -X DELETE http://$name_server:3491";
code="405";
unittest "Not allowed 2";

##################################################################

{ anounce Forbidden \
\
	':3492 is solenly a `server { listen 3492; }`, \n
	this is an open port, but forbidden serverside, \n
	because it has no location, root, nor redirect. (403)' \
\
; } 2> /dev/null

cmd="curl -v http://$name_server:3492"
code="403";
unittest "Forbidden";

##################################################################

{ anounce Dumb_client \
\
	' - client NOT redirecting (gets 301):\n
	(dumb test, this is client-side).' \
\
; } 2> /dev/null

cmd="curl http://$name_server:3493"
code="301";
unittest "Client not redirecting";

##################################################################

{ anounce Redirect \
\
	':3493 server redirects 301 to :3490. \n
	- client redirecting:' \
\
; } 2> /dev/null

cmd="curl http://$name_server:3493 -L"
file="$MYDIR/confs/html/index.htm";
code="200";
unittest "Client redirecting made two calls";

## POST_1  ################################################################


{ anounce POST_1 \
\
	'POST test. Within limits of client_max_body_size:' \
\
; } 2> /dev/null

echo -n "This file is exactly 99 bytes long, and is used to test POST requests. This text is printable: EOF\n" > ${MYDIR}/99B.words
cmd="curl -F \"file=@${MYDIR}/99B.words\" http://$name_server:3490";
code="201";
unittest "Simple post";
colorprint "Compare files" "`cat ${MYDIR}/99B.words`" "`cat ${MYDIR}/confs/html/99B.words`";
rm ${MYDIR}/99B.words
ls -l ${MYDIR}/confs/html/99B.words;
cat ${MYDIR}/confs/html/99B.words;

## POST_2 ################################################################


{ anounce POST_2 \
\
	'POST test. 99B again, noise this time.' \
\
; } 2> /dev/null

head -c 99 /dev/urandom > ${MYDIR}/file.noise
cmd="curl -F \"file=@${MYDIR}/file.noise\" http://$name_server:3490";
code="201";
file="$MYDIR/confs/html/file.noise";
unittest "Simple post with noise";
rm ${MYDIR}/file.noise
ls -l ${MYDIR}/confs/html/file.noise;

finish;
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

head -c 1MiB /dev/urandom > ${MYDIR}/file.noise
curl -vL -F "file=@${MYDIR}/file.noise" http://$name_server:4242 
rm ${MYDIR}/file.noise
ls -l ${MYDIR}/confs/html4242/file.noise;

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

curl -D- --trace-ascii log -F "file=@${MYDIR}/99B.words" \
	http://$name_server:4242/post_body && cat log && rm log
ls -l ${MYDIR}/confs/html4242/uploads/99B.words
cat ${MYDIR}/confs/html4242/uploads/99B.words

## LB ###############################################################

{ anounce LB_2nd \
\
	'2nd) The same, using noise file.' \
\
; } 2> /dev/null

head -c 99 /dev/urandom > ${MYDIR}/file.noise
curl -D- --trace-ascii log -F "file=@${MYDIR}/file.noise" \
	http://$name_server:4242/post_body && cat log && rm log
rm ${MYDIR}/file.noise
ls -l ${MYDIR}/confs/html4242/uploads/file.noise;

## LB ###############################################################

{ anounce LB_3rd \
\
	'3rd) Same, with 100B.noise. Max is at 100B, this passes. \n
	Same file name, so it overwrites.'\
\
; } 2> /dev/null

head -c 100 /dev/urandom > ${MYDIR}/file.noise
curl -vF "file=@${MYDIR}/file.noise" http://$name_server:4242/post_body
rm ${MYDIR}/file.noise
ls -l ${MYDIR}/confs/html4242/uploads/file.noise;

## LB ###############################################################

{ anounce LB_4th \
\
	'4th) 101B.noise should not pass because of max_size is set for 100. 413' \
\
; } 2> /dev/null

head -c 101 /dev/urandom > ${MYDIR}/file.noise
curl -vF "file=@${MYDIR}/file.noise" http://$name_server:4242/post_body
rm ${MYDIR}/file.noise
ls -l ${MYDIR}/confs/html4242/uploads/file.noise;

## Large Uploads ################################################################

{ anounce Large_Uploads_1 \
\
	'Testing 1MiB.noise. This should be reject by 413 because exceeds max_size.' \
\
; } 2> /dev/null

head -c 1MiB /dev/urandom > ${MYDIR}/file.noise
curl -vF "file=@${MYDIR}/file.noise" http://$name_server:4242/post_body
rm ${MYDIR}/file.noise
ls -l ${MYDIR}/confs/html4242/uploads/file.noise;

## Large Uploads ################################################################

{ anounce Large_Uploads_2 \
\
	'Not large, but shows "uri /large_upload" is working. 42B. 202' \
\
; } 2> /dev/null

head -c 42 /dev/urandom > ${MYDIR}/file.noise
curl -vF "file=@${MYDIR}/file.noise" http://$name_server:4242/large_upload
rm ${MYDIR}/file.noise
ls -l ${MYDIR}/confs/html4242/uploads/file.noise;

## Large Uploads ################################################################

{ anounce Large_Uploads_3 \
\
	'Now POSTing 1MiB.noise.\n
	This shall NOT be accepted, because curl will expect 100-continue,\n
	but webserv must always close the connection. Chose 424 for answer.' \
\
; } 2> /dev/null

head -c 1MiB /dev/urandom > ${MYDIR}/file.noise
curl -vF "file=@${MYDIR}/file.noise" http://$name_server:4242/large_upload
rm ${MYDIR}/file.noise
ls -l ${MYDIR}/confs/html4242/uploads/file.noise;

## Large Uploads ################################################################

{ anounce Large_Uploads_4 \
\
	'Again 1MiB.noise, this time sending the file right away.\n
	This SHALL BE accepted and saved.' \
\
; } 2> /dev/null

head -c 1MiB /dev/urandom > ${MYDIR}/file.noise
curl -vF "file=@${MYDIR}/file.noise" -H "Expect:" http://$name_server:4242/large_upload
rm ${MYDIR}/file.noise
ls -lh ${MYDIR}/confs/html4242/uploads/file.noise;

## Large Uploads ################################################################

{ anounce Large_Uploads_5 \
\
	'Now 2MiB.noise, alse sending the file right away.' \
\
; } 2> /dev/null

head -c 2MiB /dev/urandom > ${MYDIR}/file.noise
curl -vF "file=@${MYDIR}/file.noise" -H "Expect:" http://$name_server:4242/large_upload
rm ${MYDIR}/file.noise
ls -lh ${MYDIR}/confs/html4242/uploads/file.noise;

## Large Uploads ################################################################

{ anounce Large_Uploads_6 \
\
	'How about 50MB?\n
	Testing on 42SP Workspace, it gets oom kill once ocasionally with 100MB.' \
\
; } 2> /dev/null

head -c 50MB /dev/urandom > ${MYDIR}/file.noise
curl -vF "file=@${MYDIR}/file.noise" -H "Expect:" http://$name_server:4242/large_upload
rm ${MYDIR}/file.noise
ls -l ${MYDIR}/confs/html4242/uploads/file.noise;

## Large Uploads ################################################################

{ anounce Large_Uploads_7 \
\
	'How about 200MiB? Wait a little, but this would crash on Workspace!\n
	Server should not crash, so 507 Insufficient Storage.' \
\
; } 2> /dev/null

head -c 200MiB /dev/urandom > ${MYDIR}/file.noise
curl -vF "file=@${MYDIR}/file.noise" -H "Expect:" http://$name_server:4242/large_upload
rm ${MYDIR}/file.noise
ls -l ${MYDIR}/confs/html4242/uploads/file.noise;

## Clean uploaded files  ################################################################

{ anounce Clean_Uploads \
\
	'All large uploads were multipart/form-data.\n
	Now reset test over again for chunked encoding tests.' \
\
; } 2> /dev/null
{ ${MYDIR}/clean_uploads.sh; }

## Large Uploads ################################################################


{ anounce Large_Uploads_1 \
\
	'Testing 1MiB.noise. This should be reject by 413 because exceeds max_size.' \
\
; } 2> /dev/null

head -c 1MiB /dev/urandom > ${MYDIR}/file.noise

curl -v -H "Expect:" -H "Content-Type: test/file" -H \
	"Transfer-Encoding: chunked" -d \
	"@${MYDIR}/file.noise" http://$name_server:4242/post_body/file.noise

rm ${MYDIR}/file.noise
ls -lh ${MYDIR}/confs/html4242/uploads/file.noise;

## Large Uploads ################################################################

{ anounce Large_Uploads_2 \
\
	'Not large, but shows "uri /large_upload" is working. 42B. 202' \
\
; } 2> /dev/null

head -c 42 /dev/urandom > ${MYDIR}/file.noise

curl -v -H "Expect:" -H "Content-Type: test/file" -H \
	"Transfer-Encoding: chunked" -d \
	"@${MYDIR}/file.noise" http://$name_server:4242/large_upload/file.noise

rm ${MYDIR}/file.noise
ls -lh ${MYDIR}/confs/html4242/uploads/file.noise;

## Large Uploads ################################################################

{ anounce Large_Uploads_3 \
\
	'1MiB.noise.\n
	This SHALL BE accepted and saved.' \
\
; } 2> /dev/null

head -c 1MiB /dev/urandom > ${MYDIR}/file.noise
curl -v -H "Expect:" -H "Content-Type: test/file" -H \
	"Transfer-Encoding: chunked" -d \
	"@${MYDIR}/file.noise" http://$name_server:4242/large_upload/file.noise
rm ${MYDIR}/file.noise
ls -lh ${MYDIR}/confs/html4242/uploads/file.noise;

## Large Uploads ################################################################

{ anounce Large_Uploads_4 \
\
	'Now 2MiB.noise.' \
\
; } 2> /dev/null

head -c 2MiB /dev/urandom > ${MYDIR}/file.noise
curl -v -H "Expect:" -H "Content-Type: test/file" -H \
	"Transfer-Encoding: chunked" -d \
	"@${MYDIR}/file.noise" http://$name_server:4242/large_upload/file.noise
rm ${MYDIR}/file.noise
ls -lh ${MYDIR}/confs/html4242/uploads/file.noise;

## Large Uploads ################################################################

{ anounce Large_Uploads_5 \
\
	'How about 50MB?\n
	Testing on 42SP Workspace, it gets oom kill once ocasionally with 100MB.' \
\
; } 2> /dev/null

head -c 50MB /dev/urandom > ${MYDIR}/file.noise
curl -v -H "Expect:" -H "Content-Type: test/file" -H \
	"Transfer-Encoding: chunked" -d \
	"@${MYDIR}/file.noise" http://$name_server:4242/large_upload/file.noise
rm ${MYDIR}/file.noise
ls -lh ${MYDIR}/confs/html4242/uploads/file.noise;

## Large Uploads ################################################################

{ anounce Large_Uploads_6 \
\
	'How about 200MiB? Wait a little, but this would crash on Workspace!\n
	Server should not crash, so 507 Insufficient Storage.' \
\
; } 2> /dev/null

head -c 200MiB /dev/urandom > ${MYDIR}/file.noise
curl -v -H "Expect:" -H "Content-Type: test/file" -H \
	"Transfer-Encoding: chunked" -d \
	"@${MYDIR}/file.noise" http://$name_server:4242/large_upload/file.noise
rm ${MYDIR}/file.noise
ls -lh ${MYDIR}/confs/html4242/uploads/file.noise;

exit; # < < < < < < < < < < < < < < < < < < < < < < < < < < End line!

## Stress ################################################################

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
	'Double test: make two simultaneous calls. Not mandatory.'
\
; } 2> /dev/null

curl -F "file=@${MYDIR}/99B.words" \
	http://$name_server:4242/post_body & \
curl -F "file=@${MYDIR}/99B.words" \
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

#curl -vLD- http://$name_server:4242/post_body -F 'file=./1M.noise'
#curl -vLD- http://$name_server:4242/directory/youpi.bla

#################################################################

finish
