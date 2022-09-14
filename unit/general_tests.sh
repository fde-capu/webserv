#!/bin/sh

stress_count=300;
name_server="127.0.0.1";

MYSELF="$(realpath "$0")"
MYDIR="${MYSELF%/*}"
ok_count=0;
ko_count=0;
tot_count=0;

resetvars()
{
	cmd="";
	code="";
	testfile="";
	upfile="";
	noise="";
	fail="";
	outdir="";
	chunked="";
	trace="";
	show_output="";
}
resetvars;

unittest()
{
	fullcmd="set -x; $cmd -sSvw '%{http_code}'";

	if [ "$noise" != "" ] ; then
		upfile="file.noise";
		head -c $noise /dev/urandom > "${MYDIR}/$upfile";
	fi

	if [ "$upfile" != "" ] ; then
		if [ "$chunked" = "" ] ; then
			fullcmd="$fullcmd -F \"file=@${MYDIR}/$upfile\"";
		else
			fullcmd="$fullcmd -d \"@${MYDIR}/$upfile\"";
			fullcmd="$fullcmd -H \"Expect:\" -H \"Content-Type: test/file\" -H \"Transfer-Encoding: chunked\""
		fi
	fi

	[ "$trace" != "" ] && fullcmd="$fullcmd --trace-ascii tmp_trace_ascii";
	fullcmd="$fullcmd -o tmp_response";
	
	out=`eval "$fullcmd"`;

	[ "$show_output" != "" ] && echo "<<<" && cat tmp_response && echo "<<<";

	if [ "$out" = "000" ]; then
		{ anounce ERROR 'Make sure the server is running! (Response 0?)'; } 2> /dev/null;
		exit 1;
	fi

	colorprint "$1 Code expect $code, got $out" "$out" "$code"

	if [ "$fail" = "" ] ; then
		[ "$testfile" != "" ] && colorprint "$1 Compare ouput" "`cat tmp_response`" "`cat $testfile`";
		[ "$upfile" != "" ] && colorprint "$1 Compare files" "`cat $outdir/$upfile`" "`cat ${MYDIR}/$upfile`";
	fi

	[ "$noise" != "" ] && rm ${MYDIR}/$upfile;
	rm tmp_response;
	[ "$trace" != "" ] && cat tmp_trace_ascii
	[ "$trace" != "" ] && rm tmp_trace_ascii

	resetvars;
}

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
	echo "dummy line so jump may be right below" 2> /dev/null

#################################################################### Begin


fi # > > > > > > > > > > > > > > > > > > > > > > > > > > > Jump line!


##################################################################

{ anounce Basic_1 \
\
	':3490 will demonstrate the implementations. \n
	It is bound to directory ./unit/confs/html.	\n
	Must receive 200 OK and some html body from html/index.html.' \
\
; } 2> /dev/null

cmd="curl http://$name_server:3490";
testfile="$MYDIR/confs/html/index.htm";
code="200";
unittest "Basic 1";

##################################################################

{ anounce Root_by_servername \
\
	'If server_name is existent on another root: 200' \
\
; } 2> /dev/null

cmd="curl http://$name_server:3490 -H 'Host: krazything'";
testfile="$MYDIR/confs/html-custom-server-name/index.html";
code="200";
unittest "Root by servername";

#################################################################

{ anounce Subdirectory \
\
	'Accepts subdirectory calls. \n
	-L tells curl to follow redirect. \n
	nginx returns 301 on these cases, and client folows, \n
	but webserv here is directly serving.' \
\
; } 2> /dev/null

cmd="curl -vL http://$name_server:3490/somesub"
code="200"
testfile="$MYDIR/confs/html/somesub/index.htm";
unittest "Subdirectory";

#################################################################

{ anounce SubdirectorySlash \
\
	"Subdirectory ending with '/' has the same effect. \n
	This time, curl -L is not required." \
\
; } 2> /dev/null

cmd="curl -vL http://$name_server:3490/somesub/"
code="200"
testfile="$MYDIR/confs/html/somesub/index.htm";
unittest "Subdirectory with slash (/)";

#################################################################

{ anounce Another_port \
\
	':3491, another port, another server, another folder: \n
	root ./unit/confs/html-3491.' \
\
; } 2> /dev/null

cmd="curl http://$name_server:3491"
testfile="$MYDIR/confs/html-3491/index.html";
code="200";
unittest "Port to server";

##################################################################

{ anounce Redirect \
\
	':3493 server redirects 301 to :3490. \n
	- client redirecting:' \
\
; } 2> /dev/null

cmd="curl http://$name_server:3493 -L"
testfile="$MYDIR/confs/html/index.htm";
code="200";
unittest "Client redirecting made two calls";

##################################################################
##################################################################
##################################################################

{ anounce Faulty_setup \
\
	'Existent server_name without root definition. 403' \
\
; } 2> /dev/null

cmd="curl http://$name_server:3490 -H 'Host: rootless'";
code="403";
unittest "server_name w/o root (faulty config)";

##################################################################

{ anounce Unknown_servername \
\
	'If server_name is unexistent, defaults to previous: 200' \
\
; } 2> /dev/null

cmd="curl http://$name_server:3490 -H 'Host: wtf_server'";
testfile="$MYDIR/confs/html/index.htm";
code="200";
unittest "Unexistent server_name defaults to first";

##################################################################

{ anounce GET_refusal \
\
	':3491 accepts only GET. Testing POST is not allowed. 405' \
\
; } 2> /dev/null

cmd="curl http://$name_server:3491";
code="405";
noise="1MiB";
fail="true";
unittest "POST rejection";

##################################################################

{ anounce DELETE_refusal \
\
	':3491 accepts only GET. Testing DELETE is not allowed. 405' \
\
; } 2> /dev/null

cmd="curl -X DELETE http://$name_server:3491";
code="405";
fail="true";
unittest "DELETE rejection";

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
fail="true";
unittest "Forbidden (faulty config)";

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
##################################################################
##################################################################

{ anounce POST_MULTIPART \
\
	'POST multipart/form-data tests. \n
	Nginx does not support this and will fail all POST tests.\n
	Within limits of client_max_body_size:\nTraced.' \
\
; } 2> /dev/null

echo -n "This file is exactly 99 bytes long, and is used to test POST requests. This text is printable: EOF!\n" > ${MYDIR}/99B.words
cmd="curl http://$name_server:3490";
outdir="${MYDIR}/confs/html/";
upfile="99B.words" 
code="201";
trace="true"
unittest "Simple post";
ls -l ${MYDIR}/confs/html/99B.words;
rm ${MYDIR}/99B.words
cat ${MYDIR}/confs/html/99B.words;

##################################################################

{ anounce MULTIPART \
\
	'POST test. 99B again, noise this time.' \
\
; } 2> /dev/null

noise="99"
outdir="${MYDIR}/confs/html";
cmd="curl http://$name_server:3490";
code="201";
unittest "Simple post with noise";

##################################################################

{ anounce MULTIPART \
\
	'POST multipart noise 100B.' \
\
; } 2> /dev/null

noise="100"
outdir="${MYDIR}/confs/html";
cmd="curl http://$name_server:3490";
code="201";
unittest "Post noise 100B";

##################################################################

{ anounce MULTIPART \
\
	'POST multipart noise 101B (fail, since max is 100).' \
\
; } 2> /dev/null

trace="true";
noise="101"
outdir="${MYDIR}/confs/html";
cmd="curl http://$name_server:3490";
code="413";
fail="true";
unittest "Post noise 101B";

##################################################################

{ anounce MULTIPART \
\
	'Not large, but shows "uri /large_upload" is working. 42B. 201' \
\
; } 2> /dev/null

noise="42"
outdir="${MYDIR}/confs/html/uploads_large";
cmd="curl http://$name_server:3490/large_upload"
code="201"
unittest "Noise to large_upload 42B"

#####################################################################

{ anounce MULTIPART \
\
	'Again 1MiB.noise, this time sending the file right away.\n
	This SHALL BE accepted and saved.' \
\
; } 2> /dev/null

noise="1MiB"
cmd="curl -H \"Expect:\" http://$name_server:3490/large_upload"
outdir="${MYDIR}/confs/html/uploads_large";
code="201"
unittest "1MiB success"

#####################################################################

{ anounce MULTIPART \
\
	'Now 2MiB.noise, sending the file right away.' \
\
; } 2> /dev/null

noise="2MiB"
cmd="curl -H \"Expect:\" http://$name_server:3490/large_upload"
outdir="${MYDIR}/confs/html/uploads_large";
code="201"
unittest "2MiB success"

#####################################################################

{ anounce MULTIPART \
\
	'How about 50MB?\n
	Testing on 42SP Workspace, it gets oom kill once ocasionally with 100MB.' \
\
; } 2> /dev/null

noise="50MB"
cmd="curl -H \"Expect:\" http://$name_server:3490/large_upload"
outdir="${MYDIR}/confs/html/uploads_large";
code="201"
unittest "50MB success"

#####################################################################
#####################################################################
#####################################################################

{ anounce MULTIPART_FAIL \
\
	'Now POSTing 1MiB.noise.\n
	This shall NOT be accepted, because curl will expect 100-continue,\n
	but webserv must always close the connection. Chose 424 for answer.' \
\
; } 2> /dev/null

noise="1MiB"
cmd="curl http://$name_server:3490/large_upload"
code="424"
fail="true"
unittest "webserv must close connection"

#####################################################################

{ anounce MULTIPART_FAIL \
\
	'How about 200MiB? Wait a little, but this would get OOM KILL\n
	by Workspace. Set CIRCULARBUFFER_LIMIT for this.\n
	Found safe to stick to 1048576 \* 100.\n
	Server should not crash, so 507 Insufficient Storage.' \
\
; } 2> /dev/null

noise="200MB"
cmd="curl -H \"Expect:\" http://$name_server:3490/large_upload"
code="507"
fail="true"
unittest "200MB rejection (out of resources)"

##################################################################
##################################################################
##################################################################

{ anounce Clean_Uploads \
\
	'All large uploads were multipart/form-data.\n
	Now reset test over again for chunked encoding tests.' \
\
; } 2> /dev/null
{ ${MYDIR}/clean_uploads.sh; }

##################################################################
##################################################################
##################################################################


{ anounce POST_CHUNKED \
\
	'Test sending a text file in chunked mode. \n
	As it is not CGI, webserv refuses (501).' \
\
; } 2> /dev/null

echo -n "This file is exactly 99 bytes long, and is used to test POST requests. This text is printable: EOF!\n" > ${MYDIR}/99B.words
cmd="curl http://$name_server:3490/99B.words";
outdir="${MYDIR}/confs/html/";
upfile="99B.words" 
chunked="true"
code="201";
unittest "Simple post chunked";
ls -l ${MYDIR}/confs/html/99B.words;
rm ${MYDIR}/99B.words
cat ${MYDIR}/confs/html/99B.words;

###################################################################


{ anounce POST_CHUNKED \
\
	'But if the chunk is configured to run some CGI, it should \n
	run and return the proper body.' \
\
; } 2> /dev/null

echo -n "This file is exactly 99 bytes long, and is used to test POST requests. This text is printable: EOF!\n" > ${MYDIR}/99B.bla
cmd="curl http://$name_server:3490/99B.bla";
outdir="${MYDIR}/confs/html/";
upfile="99B.bla" 
chunked="true"
code="202";
show_output="true";
unittest "Simple post chunked calling CGI";
ls -l ${MYDIR}/confs/html/99B.bla;
rm ${MYDIR}/99B.bla
cat ${MYDIR}/confs/html/99B.bla;

###################################################################
{ anounce CHUNKED \
\
	'99B using noise file.' \
\
; } 2> /dev/null

chunked="true";
noise="99"
outdir="${MYDIR}/confs/html";
cmd="curl http://$name_server:3490";
code="201";
unittest "Simple post with noise";

#################################################################

{ anounce CHUNKED \
\
	'Same, with 100B.noise. Max is at 100B, this passes. \n
	Same file name, so it overwrites.'\
\
; } 2> /dev/null

chunked="true";
noise="100"
outdir="${MYDIR}/confs/html";
cmd="curl http://$name_server:3490";
code="201";
unittest "Post noise 100B";

#################################################################

{ anounce CHUNKED \
\
	'101B.noise should not pass because of max_size is set for 100. 413' \
\
; } 2> /dev/null

chunked="true"
noise="101"
outdir="${MYDIR}/confs/html";
cmd="curl http://$name_server:3490";
code="403";
unittest "Post noise 101B";

###################################################################

{ anounce CHUNKED \
\
	'Not large, but shows "uri /large_upload" is working. 42B. 202' \
\
; } 2> /dev/null

chunked="true";
noise="42"
outdir="${MYDIR}/confs/html/uploads_large";
cmd="curl http://$name_server:3490/large_upload"
code="201"
unittest "Noise to large_upload 42B"

###############################################################

{ anounce CHUNKED \
\
	'1MiB.noise.\n
	This SHALL BE accepted and saved.' \
\
; } 2> /dev/null

chunked="true"
noise="1MiB"
cmd="curl -H \"Expect:\" http://$name_server:3490/large_upload"
outdir="${MYDIR}/confs/html/uploads_large";
code="201"
unittest "1MiB success"

##################################################################

{ anounce CHUNKED \
\
	'Now 2MiB.noise.' \
\
; } 2> /dev/null

chunked="true";
noise="2MiB"
cmd="curl -H \"Expect:\" http://$name_server:3490/large_upload"
outdir="${MYDIR}/confs/html/uploads_large";
code="201"
unittest "2MiB success"

##################################################################

{ anounce CHUNKED \
\
	'How about 50MB?\n
	Testing on 42SP Workspace, it gets oom kill once ocasionally with 100MB.' \
\
; } 2> /dev/null

chunked="true";
noise="50MB"
cmd="curl -H \"Expect:\" http://$name_server:3490/large_upload"
outdir="${MYDIR}/confs/html/uploads_large";
code="201"
unittest "50MB success"

##################################################################
#####################################################################
#####################################################################

{ anounce CHUNKED_FAIL \
\
	'Now POSTing 1MiB.noise.\n
	This shall NOT be accepted, because curl will expect 100-continue,\n
	but webserv must always close the connection. Chose 424 for answer.' \
\
; } 2> /dev/null

chunked="true";
noise="1MiB"
cmd="curl http://$name_server:3490/large_upload"
code="424"
fail="true"
unittest "webserv must close connection"

#####################################################################

{ anounce CHUNKED_FAIL \
\
	'How about 200MiB? Wait a little, but this would crash on Workspace!\n
	Server should not crash, so 507 Insufficient Storage.' \
\
; } 2> /dev/null

chunked="true";
noise="200MB"
cmd="curl -H \"Expect:\" http://$name_server:3490/large_upload"
code="507"
fail="true"
unittest "200MB rejection (out of resources)"

finish; # < < < < < < < < < < < < < < < < < < < < < < < < < < End line!

##################################################################
##################################################################
##################################################################

{ anounce CGI_1 \
\
	'Test CGI.' \
\
; } 2> /dev/null

echo -n "This file is exactly 99 bytes long, and is used to test POST requests. This text is printable: EOF!\n" > ${MYDIR}/99B.words
upfile="99B.words" 
chunked="true"
cmd="curl http://$name_server:4242/directory/youpi.bla"
code="201"
trace="true"
outdir="${MYDIR}/confs/html4242/uploads";
fail="true"
unittest "First CGI"
ls -l ${MYDIR}/confs/html4242/uploads/99B.words;
rm ${MYDIR}/99B.words
cat ${MYDIR}/confs/html4242/uploads/99B.words;

##################################################################
##################################################################
##################################################################

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
#################################################################

#################################################################
## ubuntu_tester specifics
##################################################################

{ anounce 42SP \
\
	'POST :4242 at root (/) should fail, 405 not allowed.' \
\
; } 2> /dev/null

noise="1MiB"
cmd="curl http://$name_server:4242 "
code="405";
fail="true";
unittest "Post fail"

#####################################################################

{ anounce 42SP \
\
	'DELETE :4242 at root (/) also should fail. 405' \
\
; } 2> /dev/null

cmd="curl -vL -X DELETE http://$name_server:4242"
code="405"
unittest "Delete reject"

#####################################################################

{ anounce 42SP \
\
	'/post_body rejects all but POST (2 tests). 405' \
\
; } 2> /dev/null

cmd="curl -X GET http://$name_server:4242/post_body"
code="405"
unittest "Reject GET"

{ div; } 2> /dev/null

cmd="curl -X DELETE http://$name_server:4242/post_body"
code="405"
unittest "Reject GET"

#####################################################################

{ anounce 42SP \
\
	'- /post_body must answer anything to POST request with a \n
	maxBody of 100. Many tests.\n
	- 1st) Post 99B.words into html4242/uploads (as set in config).' \
\
; } 2> /dev/null

echo -n "This file is exactly 99 bytes long, and is used to test POST requests. This text is printable: EOF!\n" > ${MYDIR}/99B.words
cmd="curl http://$name_server:4242/post_body";
outdir="${MYDIR}/confs/html4242/uploads";
upfile="99B.words" 
code="201";
unittest "Post to 4242 uploads";
ls -l ${MYDIR}/confs/html4242/uploads/99B.words;
rm ${MYDIR}/99B.words
cat ${MYDIR}/confs/html4242/uploads/99B.words;

#####################################################################

{ anounce 42SP \
\
	'2nd) The same, using noise upfile.' \
\
; } 2> /dev/null

noise="99"
cmd="curl http://$name_server:4242/post_body"
outdir="${MYDIR}/confs/html4242/uploads";
code="201"
unittest "Noise 99"

#####################################################################

{ anounce 42SP \
\
	'3rd) Same, with 100B.noise. Max is at 100B, this passes. \n
	Same file name, so it overwrites.'\
\
; } 2> /dev/null

noise="100"
cmd="curl http://$name_server:4242/post_body"
outdir="${MYDIR}/confs/html4242/uploads";
code="201"
unittest "Noise 100"

#####################################################################

{ anounce 42SP \
\
	'4th) 101B.noise should not pass because of max_size is set for 100. 413' \
\
; } 2> /dev/null

noise="101"
cmd="curl http://$name_server:4242/post_body"
outdir="${MYDIR}/confs/html4242/uploads";
code="413"
fail="true"
unittest "Noise 101"

#####################################################################

finish

#####################################################################
#####################################################################
#####################################################################
# Util commands:

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
