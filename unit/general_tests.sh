#!/bin/sh
# XXX --resolve method
# XXX open ports
# XXX redirect URL

# unit test for webserv
# by fde-capu

# All variables are true if some string "anything" and false as empty string.

name_server="127.0.0.1";
step_by_step="";
clean_upfiles_after_test="";
silent="yes";
ultrasilent="";

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
	plaintext="";
	trace="";
	show_output="";
	short_output="";
	compare_size="";
	message="";
}
resetvars;

unittest()
{
	fullcmd="$cmd";

	if [ "$noise" != "" ] ; then
		upfile="file.noise";
		head -c $noise /dev/urandom > "${MYDIR}/$upfile";
		[ "$silent" = "" ] && [ "$ultrasilent" = "" ] && echo "upfile:" && ls -l "${MYDIR}/$upfile";
		[ "$chunked" != "" ] && fullcmd="$fullcmd/$upfile";
	fi

	fullcmd="set -x; $fullcmd -sSvw '%{http_code}'";

	if [ "$upfile" != "" ] ; then
		if [ "$chunked" = "" ] ; then
			fullcmd="$fullcmd -F \"file=@${MYDIR}/$upfile\"";
		else
			fullcmd="$fullcmd --data-binary \"@${MYDIR}/$upfile\"";
			fullcmd="$fullcmd -H \"Expect:\" -H \"Content-Type: test/file\" -H \"Transfer-Encoding: chunked\""
		fi
	else
		if [ "$plaintext" != "" ] ; then
			fullcmd="$fullcmd -H \"Content-Type: plain/text\" -d \"$plaintext\"";
		fi
	fi

	[ "$trace" != "" ] && fullcmd="$fullcmd --trace-ascii tmp_trace_ascii";
	fullcmd="$fullcmd -o tmp_response";
	if [ "$silent" != "" ] || [ "$ultrasilent" != "" ] ; then
		fullcmd="$fullcmd 2> /dev/null";
		out=`eval "$fullcmd" 2> /dev/null`;
	else
		out=`eval "$fullcmd"`;
	fi

	if [ "$silent" = "" ] && [ "$ultrasilent" = "" ] ; then
		[ "$show_output" != "" ] && echo "<<<" && cat tmp_response && echo "<<<";
		[ "$short_output" != "" ] && ls -l tmp_response && head -c 80 tmp_response && echo "...";
	fi

	if [ "$out" = "000" ]; then
		{ anounce ERROR 'Make sure the server is running!'; } 2> /dev/null;
		exit 1;
	fi

	[ "$fail" = "" ] && [ "$testfile" != "" ] && [ "$silent" = "" ] && [ "$ultrasilent" = "" ] && echo "File comparison: $testfile";

	colorscore "$1 | expect $code, got $out" "$out" "$code"

	if [ "$fail" = "" ] ; then
		[ "$testfile" != "" ] && colorscore "$1 | compare ouput" "`cat tmp_response`" "`cat $testfile`";
		[ "$outdir" != "" ] && [ "$upfile" != "" ] && colorscore "$1 | compare files" "`cat $outdir/$upfile`" "`cat ${MYDIR}/$upfile`";
	fi

	if [ "$compare_size" != "" ] ; then
		colorscore "$1 | compare sizes" "`ls -l tmp_response | awk '{print $5}'`" "$compare_size";
	fi

	[ "$trace" != "" ] && [ "$silent" = "" ] && [ "$ultrasilent" = "" ] && cat tmp_trace_ascii;
	[ "$message" != "" ] && [ "$silent" = "" ] && [ "$ultrasilent" = "" ] && echo "\033[0;33m$message\033[0;37m";

	[ "$clean_upfiles_after_test" != "" ] && [ "$noise" != "" ] && rm -f ${MYDIR}/$upfile;
	[ "$clean_upfiles_after_test" != "" ] && [ "$trace" != "" ] && rm -f tmp_trace_ascii;
	[ "$clean_upfiles_after_test" != "" ] && rm -f tmp_response;

	resetvars;
}

run()
{
	fullcmd="$1";
	if [ "$silent" != "" ] || [ "$ultrasilent" != "" ] ; then
		fullcmd="$fullcmd 2> /dev/null";
		out=`eval "$fullcmd" 2> /dev/null`;
	else
		out=`eval "$fullcmd"`;
	fi
}

stressupmulti()
{
	sleep 4
	{ anounce STRESS_UPLOADS_MULTI \
		\
			"Stress testing $stress_count uploads simultanouslly of $noise_size.\n
			 Needs score of $more_than or more." \
			\
			; } 2> /dev/null

		rm -f stress_out;
		head -c $noise_size /dev/urandom > file.noise;
		[ "$silent" = "" ] && [ "$ultrasilent" = "" ] && echo "MULTIPART | $stress_count x $noise_size";
		[ "$silent" != "" ] && echo -n "MULTIPART | $stress_count x $noise_size | ";
		i=1;
		while [ "$i" -le "$stress_count" ]; do
			[ "$silent" = "" ] && [ "$ultrasilent" = "" ] && echo -n "\r $i ";
			curl -H "Expect:" http://127.0.0.1:3490/large_upload -sv -F file=@./file.noise 2>> stress_out 1> /dev/null &
			i=$(( i + 1 ));
		done;
		wait;
		stress_result=$(cat stress_out | grep HTTP | grep "201 Created" | wc -l);
		colorge "Total $stress_count, accept $more_than, score $stress_result" "$stress_result" "$more_than";
		rm -f stress_out;
		rm -f file.noise;
}

stressupchunk()
{
	sleep 4
	{ anounce STRESS_UPLOADS_CHUNK \
		\
			"Stress testing $stress_count chunked uploads simultanouslly of $noise_size.\n
			 Needs score of $more_than or more." \
			\
			; } 2> /dev/null

		rm -f stress_out;
		head -c $noise_size /dev/urandom > file.noise;
		[ "$silent" = "" ] && [ "$ultrasilent" = "" ] && echo "CHUNKED | $stress_count x $noise_size";
		[ "$silent" != "" ] && echo -n "CHUNKED | $stress_count x $noise_size | ";
		i=1;
		while [ "$i" -le "$stress_count" ]; do
			sleep 0.05
			[ "$silent" = "" ] && [ "$ultrasilent" = "" ] && echo -n "\r $i ";
			curl -H "Expect:" -H "Content-Type: test/file" -H "Transfer-Encoding: chunked" http://127.0.0.1:3490/large_upload/file.nois -sv -F file=@./file.noise 2>> stress_out 1> /dev/null &
			i=$(( i + 1 ));
		done;
		wait;
		stress_result=$(cat stress_out | grep HTTP | grep "201 Created" | wc -l);
		colorge "Total $stress_count, accept $more_than, score $stress_result" "$stress_result" "$more_than";
		rm -f stress_out;
		rm -f file.noise;
}

enterkey()
{
	[ "$step_by_step" = "" ] && return;
	echo -n "\t\t\t\t\t--> Next ";
	read anything;
}

div()
{
	echo "\n\n # # # # # #\n"
}

divider()
{
	if [ "$ultrasilent" = "" ] ; then
		[ "$silent" = "" ] && echo "\n\n######### $1 #####################################";
	fi
}

anounce()
{
	enterkey;
	if [ "$ultrasilent" = "" ] ; then
		if [ "$silent" = "" ] ; then
			divider $1;
			echo $2;
			echo '-------------------------------------------------';
		fi
	fi
	passtitle="$1";
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
	clean;
	{ anounce FINISHED \
	\
		'..........now........\n
		\t......the.....\n\t\t....results......' \
	\
	; } 2> /dev/null
	[ "$silent" != "" ] && echo "";
	[ "$ultrasilent" = "" ] && echo " Total:\t\t $tot_count";
	[ "$ultrasilent" = "" ] && echo "\033[0;32m [ OK ] \t $ok_count \033[0;37m";
	if [ "$ko_count" = "0" ] ; then
		echo "\033[0;32m Nice! \033[0;37m\n";
	else
		echo "\033[0;31m [ KO ] \t $ko_count \t:(\033[0;37m";
	fi;
	exit 0;
}

colorscore()
{
	[ "$silent" != "" ] && [ "$ultrasilent" = "" ] && echo -n "$passtitle | ";
	a=`echo "$2"`;
	b=`echo "$3"`;
	[ "$ultrasilent" = "" ] && echo -n "$1 ";
	if [ "$a" = "$b" ] ; then
		printf "\e[32m%s\e[37m " "[ OK ]";
		ok_count=$((ok_count+1));
	else
		printf "\e[31m%s\e[37m " "[ KO ]";
		ko_count=$((ko_count+1));
	fi
	tot_count=$((tot_count+1));
	[ "$ultrasilent" = "" ] && echo "";
}

colorge()
{
	a=`echo "$2"`;
	b=`echo "$3"`;
	[ "$ultrasilent" = "" ] && echo -n "$1 ";
	if [ "$a" -ge "$b" ] ; then
		printf "\e[32m%s\e[37m " "[ OK ]";
		ok_count=$((ok_count+1));
	else
		printf "\e[31m%s\e[37m " "[ KO ]";
		ko_count=$((ko_count+1));
	fi
	tot_count=$((tot_count+1));
	[ "$ultrasilent" = "" ] && echo "";
}

list()
{
	[ "$ultrasilent" = "" ] && [ "$silent" = "" ] && ls -l "$1";
}

dog()
{
	[ "$ultrasilent" = "" ] && [ "$silent" = "" ] && cat "$1";
}

say()
{
	[ "$ultrasilent" = "" ]  && [ "$silent" = "" ] && echo "$1";
}

for i in 1 2 3 4 5 6 7 8 9 10
do
	{ divider "#"; } 2> /dev/null
done

clean()
{
	[ "$ultrasilent" = "" ] && [ "$silent" = "" ] && anounce "CLEAN" 'Clean test files.' && { ${MYDIR}/clean_uploads.sh; }
	if [ "$ultrasilent" = "" ] ; then
		[ "$silent" != "" ] && { ${MYDIR}/clean_uploads.sh; } > /dev/null
	else
		{ ${MYDIR}/clean_uploads.sh; } > /dev/null
	fi
}

{ set +x; } 2> /dev/null;

clean
if false; then
	echo "dummy line so jump may be right below" 2> /dev/null

############################################################### Begin

fi # > > > > > > > > > > > > > > > > > > > > > > > Jump line!

##################################################################

{ anounce BASIC_ONE \
\
	':3490 will demonstrate the implementations. \n
	It is bound to directory ./unit/confs/html.	\n
	Must receive 200 OK and some html body from html/index.html.' \
\
; } 2> /dev/null

cmd="curl http://$name_server:3490";
testfile="$MYDIR/confs/html/index.htm";
code="200";
show_output="true";
unittest "Basic one";

##################################################################

{ anounce ROOT_BY_HOSTNAME \
\
	'If server_name is existent on another root: 200' \
\
; } 2> /dev/null

cmd="curl http://$name_server:3490 -H 'Host: krazything'";
testfile="$MYDIR/confs/html-custom-server-name/index.html";
code="200";
show_output="true";
unittest "Root by servername";

#################################################################

{ anounce SUBDIRECTORY \
\
	'Accepts subdirectory calls.' \
\
; } 2> /dev/null

cmd="curl -v http://$name_server:3490/somesub"
code="200"
testfile="$MYDIR/confs/html/somesub/index.htm";
show_output="true";
unittest "Subdirectory";

#################################################################

{ anounce SUBSDIRECTORY_SLASH \
\
	"Subdirectory ending with '/' has the same effect." \
\
; } 2> /dev/null

cmd="curl -v http://$name_server:3490/somesub/"
code="200"
testfile="$MYDIR/confs/html/somesub/index.htm";
show_output="true";
unittest "Subdirectory with slash (/)";

#################################################################

{ anounce ANOTHER_PORT \
\
	':3491, another port, another server, another folder: \n
	root ./unit/confs/html-3491.' \
\
; } 2> /dev/null

cmd="curl http://$name_server:3491"
testfile="$MYDIR/confs/html-3491/index.html";
code="200";
show_output="true";
unittest "Port to server";

##################################################################

{ anounce DUMB_CLIENT \
\
	' - client NOT redirecting (gets 301):\n
	(dumb test, this is client-side).' \
\
; } 2> /dev/null

cmd="curl http://$name_server:3493"
code="301";
unittest "Client not redirecting";

##################################################################

{ anounce REDIRECT \
\
	':3493 server redirects 301 to :3490. \n
	- client redirecting makes two calls:' \
\
; } 2> /dev/null

cmd="curl -L http://$name_server:3493"
testfile="$MYDIR/confs/html/index.htm";
code="200";
unittest "Client redirecting";

##################################################################
##################################################################
##################################################################

{ anounce GET_REFUSAL \
\
	':3491 accepts only GET. Testing POST is not allowed. 405' \
\
; } 2> /dev/null

cmd="curl http://$name_server:3491";
code="405";
noise="1MB";
fail="true";
unittest "POST rejection";

##################################################################

{ anounce DELETE_REFUSAL \
\
	':3491 accepts only GET. Testing DELETE is not allowed. 405' \
\
; } 2> /dev/null

cmd="curl -X DELETE http://$name_server:3491";
code="405";
fail="true";
unittest "DELETE rejection";

##################################################################

{ anounce UNKNOWN_METHOD \
\
	':3491 Unknow method, returns Not Implemented.' \
\
; } 2> /dev/null

cmd="curl -X WTFMETHOD http://$name_server:3491";
code="501";
fail="true";
unittest "Unknown method";

##################################################################

{ anounce NOT_THIS_DIRECTORY
\
	'Forbid existent server_name without root definition. 403' \
\
; } 2> /dev/null

cmd="curl http://$name_server:3490 -H 'Host: rootless'";
code="403";
unittest "server_name w/o root (forbidden)";

##################################################################

{ anounce UNKNOWN_SERVERNAME \
\
	'If server_name is unexistent, defaults to previous: 200' \
\
; } 2> /dev/null

cmd="curl http://$name_server:3490 -H 'Host: wtf_server'";
testfile="$MYDIR/confs/html/index.htm";
code="200";
unittest "Unexistent server_name defaults to first";

##################################################################

{ anounce FORBIDDEN \
\
	':3492 is solenly a `server { listen 3492; }`, \n
	this is an open port, but forbidden serverside, \n
	because it has no location, root, nor redirect. (403)' \
\
; } 2> /dev/null

cmd="curl -v http://$name_server:3492"
code="403";
fail="true";
unittest "Forbidden";

##################################################################
##################################################################
##################################################################

{ anounce PLAIN_TEXT_POST \
\
	'Basic plain/text.' \
\
; } 2> /dev/null

plaintext="This file is exactly 99 bytes long, and is used to test POST requests. This text is printable: EOF!";
cmd="curl http://$name_server:3490/99B.words";
outdir="${MYDIR}/confs/html/";
code="201";
unittest "Simple post multipart";
list "${MYDIR}/confs/html/99B.words";
dog "${MYDIR}/confs/html/99B.words";
rm "${MYDIR}/confs/html/99B.words";

##################################################################

{ anounce MULTI_99_WORDS \
\
	'POST multipart/form-data tests. \n
	Within limits of client_max_body_size:' \
\
; } 2> /dev/null

echo -n "This file is exactly 99 bytes long, and is used to test POST requests. This text is printable: EOF!" > ${MYDIR}/99B.words
cmd="curl http://$name_server:3490";
outdir="${MYDIR}/confs/html/";
upfile="99B.words" 
code="201";
unittest "Simple post multipart";
rm ${MYDIR}/99B.words
list "${MYDIR}/confs/html/99B.words";
dog "${MYDIR}/confs/html/99B.words";
rm "${MYDIR}/confs/html/99B.words";

##################################################################

{ anounce MULTI_99_NOISE \
\
	'POST test. 99B again, noise this time.' \
\
; } 2> /dev/null

noise="99"
outdir="${MYDIR}/confs/html";
cmd="curl http://$name_server:3490";
code="201";
unittest "Multipart post with noise";

##################################################################

{ anounce MULTI_100_NOISE \
\
	'POST multipart noise 100B.' \
\
; } 2> /dev/null

noise="100"
outdir="${MYDIR}/confs/html";
cmd="curl http://$name_server:3490";
code="201";
unittest "Multipart noise 100B";

##################################################################

{ anounce MULTI_99_NOISE_AGAIN \
\
	'POST test. 99B again, this time file has to have been overidden.' \
\
; } 2> /dev/null

noise="99"
outdir="${MYDIR}/confs/html";
cmd="curl http://$name_server:3490";
code="201";
unittest "Multipart post noise truncate file";

##################################################################

{ anounce MULTI_101_NOISE \
\
	'POST multipart noise 101B (fail, since max is 100).' \
\
; } 2> /dev/null

noise="101"
outdir="${MYDIR}/confs/html";
cmd="curl http://$name_server:3490";
code="413";
fail="true";
unittest "Multipart noise 101B";

##################################################################

{ anounce MULTI_LARGE_42 \
\
	'Not large, but shows "uri /large_upload" is working. 42B. 201' \
\
; } 2> /dev/null

noise="42"
outdir="${MYDIR}/confs/html/uploads_large";
cmd="curl http://$name_server:3490/large_upload"
code="201"
unittest "Noise to large_upload 42B"
list "${MYDIR}/confs/html/uploads_large/file.noise";

#####################################################################

{ anounce MULTI_1M_NOISE \
\
	'Again 1MB.noise, this time sending the file right away.\n
	This SHALL BE accepted and saved.' \
\
; } 2> /dev/null

noise="1MB"
cmd="curl -H \"Expect:\" http://$name_server:3490/large_upload"
outdir="${MYDIR}/confs/html/uploads_large";
code="201"
unittest "1MB success"
list "${MYDIR}/confs/html/uploads_large/file.noise";

#####################################################################

{ anounce MULTI_2M_NOISE \
\
	'Now 2MB.noise, sending the file right away.' \
\
; } 2> /dev/null

noise="2MB"
cmd="curl -H \"Expect:\" http://$name_server:3490/large_upload"
outdir="${MYDIR}/confs/html/uploads_large";
code="201"
unittest "2MB success"
list "${MYDIR}/confs/html/uploads_large/file.noise";

#####################################################################

{ anounce MULTI_50M_NOISE \
\
	'How about 50MB?\n
	Beware oom kills.' \
\
; } 2> /dev/null

rm -f ${MYDIR}/confs/html/uploads_large/file.noise
noise="50MB"
cmd="curl -H \"Expect:\" http://$name_server:3490/large_upload"
outdir="${MYDIR}/confs/html/uploads_large";
code="201"
unittest "50MB success"
list "${MYDIR}/confs/html/uploads_large/file.noise";

#####################################################################
#####################################################################
#####################################################################

{ anounce MULTI_FAIL_70MB \
\
	'Large file 70MB fail on multipart. webserv is limited to 64MB.' \
\
; } 2> /dev/null

noise="70MB"
cmd="curl -H \"Expect:\" http://$name_server:3490/large_upload"
code="507"
fail="true"
unittest "70MB multi-part (fail)"
list "${MYDIR}/confs/html/uploads_large/file.noise";

#####################################################################

{ anounce MULTI_FAIL_NO_CONT \
\
	'Now POSTing 1MB.noise.\n
	This shall NOT be accepted, because curl will expect 100-continue,\n
	but webserv must always close the connection. Chose 424 for answer.' \
\
; } 2> /dev/null

noise="1MB"
cmd="curl http://$name_server:3490/large_upload"
code="424"
fail="true"
message="Response code might have been chosen differently."
unittest "webserv close connection"

##################################################################
##################################################################
##################################################################

{ [ "$silent" != "" ] && anounce Clean_Uploads \
\
	'All large uploads were multipart/form-data.\n
	Now reset test over again for chunked encoding tests.' \
\
; } 2> /dev/null

clean;

###################################################################
###################################################################
###################################################################

{ anounce PLAIN_TEXT_FAIL \
\
	'Plain/text fails without file name.' \
\
; } 2> /dev/null

plaintext="This file is exactly 99 bytes long, and is used to test POST requests. This text is printable: EOF!";
cmd="curl http://$name_server:3490/"; # <------ No file name!
outdir="${MYDIR}/confs/html/";
code="400";
unittest "POST plain text Bad Request";

##################################################################
{ anounce CHUNK_99_WORDS_FAIL \
\
	'POST chunked tests. \n
	This rejects, because post does not specify a filename.' \
\
; } 2> /dev/null

chunked="true";
echo -n "This file is exactly 99 bytes long, and is used to test POST requests. This text is printable: EOF!" > ${MYDIR}/99B.words
cmd="curl http://$name_server:3490"; # <------- No file name!
outdir="${MYDIR}/confs/html/";
upfile="99B.words" 
code="400";
fail="true";
unittest "Chunked fail 400 Bad Request";
rm ${MYDIR}/99B.words

##################################################################

{ anounce CHUNK_99_WORDS \
\
	'POST chunked tests. \n
	Within limits of client_max_body_size:' \
\
; } 2> /dev/null

chunked="true";
echo -n "This file is exactly 99 bytes long, and is used to test POST requests. This text is printable: EOF!" > ${MYDIR}/99B.words
cmd="curl http://$name_server:3490/99B.words";
outdir="${MYDIR}/confs/html/";
upfile="99B.words" 
code="201";
unittest "Simple post";
list "${MYDIR}/confs/html/99B.words;";
rm ${MYDIR}/99B.words
dog ${MYDIR}/confs/html/99B.words;

##################################################################

{ anounce CHUNK_99_NOISE \
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

{ anounce CHUNK_100_NOISE \
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
unittest "Chunked noise 100B";

#################################################################

{ anounce CHUNK_101_NOISE \
\
	'101B.noise should not pass because of max_size is set for 100. 413' \
\
; } 2> /dev/null

chunked="true"
noise="101"
outdir="${MYDIR}/confs/html";
cmd="curl http://$name_server:3490";
code="413";
fail="true"
unittest "Chunked noise 101B";

###################################################################

{ anounce CHUNK_LARGE_42 \
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
list "${MYDIR}/confs/html/uploads_large/file.noise";

###############################################################

{ anounce CHUNK_1M_NOISE \
\
	'1MB.noise.\n
	This SHALL BE accepted and saved.' \
\
; } 2> /dev/null

chunked="true"
noise="1MB"
cmd="curl http://$name_server:3490/large_upload"
outdir="${MYDIR}/confs/html/uploads_large";
code="201"
unittest "1MB success"
list "${MYDIR}/confs/html/uploads_large/file.noise";

##################################################################

{ anounce CHUNK_2M_NOISE \
\
	'Now 2MB.noise.' \
\
; } 2> /dev/null

chunked="true";
noise="2MB"
cmd="curl http://$name_server:3490/large_upload"
outdir="${MYDIR}/confs/html/uploads_large";
code="201"
unittest "2MB success"
list "${MYDIR}/confs/html/uploads_large/file.noise";

##################################################################

{ anounce CHUNK_50M_NOISE \
\
	'How about 50MB?\n
	This takes long, because of chunk decoding.' \
\
; } 2> /dev/null

chunked="true";
noise="50MB"
cmd="curl http://$name_server:3490/large_upload"
outdir="${MYDIR}/confs/html/uploads_large";
code="201"
unittest "Chunked 50MB success"
list "${MYDIR}/confs/html/uploads_large/file.noise";

#####################################################################

{ anounce CHUNK_PARTIAL \
\
	'How about 70MB? This time, it is accepted as partial upload, \n
	since webserv is limited to 64M.' \
\
; } 2> /dev/null

chunked="true";
noise="70MB"
cmd="curl http://$name_server:3490/large_upload"
code="201"
fail="true"
unittest "Accepts, though incomplete"
list "${MYDIR}/confs/html/uploads_large/file.noise";

##################################################################
##################################################################
##################################################################

{ anounce GET_LARGE_FILE \
\
	'Gets a large file.' \
\
; } 2> /dev/null

head -c 56789012 /dev/urandom > ${MYDIR}/confs/html/uploads_large/noise_to_get

cmd="curl http://$name_server:3490/large_upload/noise_to_get";
testfile="$MYDIR/confs/html/uploads_large/noise_to_get";
code="200";
short_output="true";
unittest "Get large file";

##################################################################
##################################################################
##################################################################

{ anounce CGI_GET_SH \
\
	'Test CGI with GET method, cgi_test.sh.' \
\
; } 2> /dev/null;

cmd="curl http://$name_server:3490/cgi_test.sh"
code="200";
show_output="true";
message="There should the output of a script, not the script itself.";
unittest "Get cgi sh";

##################################################################

{ anounce CGI_GET_ON_SOMESUB \
\
	'Test CGI with GET on subdirectory calls.' \
\
; } 2> /dev/null;

cmd="curl http://$name_server:3490/somesub/hi.sh"
code="200";
show_output="true";
message="hi.sh should not show the 'echo' command (script was executed).";
unittest "Get cgi somesub/sh";

##################################################################

{ anounce CGI_GET_ON_URI_SUBSTITUTION \
\
	'Test CGI calling /uri_alias/ but getting from location root (uri_substituion).' \
\
; } 2> /dev/null;

cmd="curl http://$name_server:3490/uri_alias/hi.sh"
code="200";
show_output="true";
unittest "Get cgi alias";

##################################################################

{ anounce CGI_GET_PHP \
\
	'Test CGI with GET method, cgi_test.php.' \
\
; } 2> /dev/null;

cmd="curl http://$name_server:3490/cgi_test.php"
code="200";
show_output="true";
testfile="test_php";
echo "<-- there should NOT be and 'echo' here!!!\r\n" > "$testfile";
message="There should the output of a script, not the script itself.";
unittest "Get cgi php";
rm -f "test_php";

##################################################################

{ anounce CGI_GET_NOTFOUND \
\
	'Test CGI with GET method, xxxx.sh (unexistent).' \
\
; } 2> /dev/null;

cmd="curl http://$name_server:3490/xxxx.sh"
code="404";
unittest "Get cgi sh but no";

##################################################################
##################################################################
##################################################################

{ anounce CGI_POST_MULTI \
\
	'Test CGI call when posting multipart.' \
\
; } 2> /dev/null

echo -n "This file is exactly 99 bytes long, and is used to test POST requests. This text is printable: EOF!" > ${MYDIR}/99B.words
cmd="curl http://$name_server:3490/cgi_sort_words.sh";
upfile="99B.words" 
code="202";
show_output="true";
message="Check if CGI was properly executed above."
unittest "Simple post";
rm ${MYDIR}/99B.words

###################################################################

{ anounce CGI_POST_CHUNK \
\
	'CGI POST chunked.' \
\
; } 2> /dev/null

chunked="true"
echo -n "This file is exactly 99 bytes long, and is used to test POST requests. This text is printable: EOF!" > ${MYDIR}/99B.words
cmd="curl http://$name_server:3490/cgi_sort_words.sh";
upfile="99B.words" 
code="202";
show_output="true";
message="Check if CGI was properly executed above."
unittest "Simple post chunked";
rm ${MYDIR}/99B.words

###################################################################

{ anounce MULTI_CGI_BLA_OK \
\
	'Test CGI call when posting and calling something (existent) .bla.' \
\
; } 2> /dev/null

echo -n "This file is exactly 99 bytes long, and is used to test POST requests. This text is printable: EOF!" > ${MYDIR}/99B.words
cmd="curl http://$name_server:3490/bla.bla";
upfile="99B.words" 
code="202";
show_output="true";
message="ubuntu_cgi_tester runs and converts all to uppercase."
unittest "Simple multipart cgi post";
rm ${MYDIR}/99B.words

###################################################################

{ anounce CGI_CHUNK_BLA_OK \
\
	'Should run and return the proper body.' \
\
; } 2> /dev/null

chunked="true"
echo -n "This file is exactly 99 bytes long, and is used to test POST requests. This text is printable: EOF!" > ${MYDIR}/99B.words
cmd="curl http://$name_server:3490/bla.bla";
upfile="99B.words" 
code="202";
show_output="true";
message="Check if CGI was properly executed above."
unittest "Simple chunked cgi post";
rm ${MYDIR}/99B.words

##################################################################

{ anounce CGI_POST_MULTI_LARGE \
\
	"Test CGI call when posting multipart for large file. \n
	 Value of $largecgi is a best for not getting oom killed on Workspace." \
\
; } 2> /dev/null

largecgi="42428000";

head -c $largecgi /dev/zero | tr '\0' 'z' > "${MYDIR}/youpi.bla"
head -c $largecgi /dev/zero | tr '\0' 'Z' > "${MYDIR}/youpi_expected_result.bla"
touch "${MYDIR}/confs/html/uploads_large/bla.bla"
cmd="curl -H 'Expect:' http://$name_server:3490/large_upload/bla.bla"
upfile="youpi.bla"
code="202"
testfile="${MYDIR}/youpi_expected_result.bla"
short_output="true";
compare_size="$largecgi";
fail="true"; # comparing files would get general_tests.sh oom killed.
unittest "Test POST multipart /directory/youpi.bla large file."
rm "${MYDIR}/youpi.bla"
rm "${MYDIR}/youpi_expected_result.bla"
rm "${MYDIR}/confs/html/uploads_large/bla.bla"

###################################################################

{ anounce CGI_POST_CHUNK_LARGE \
\
	'CGI POST chunked for large file.' \
\
; } 2> /dev/null

largecgi="42428001";

chunked="true"
head -c $largecgi /dev/zero | tr '\0' 'z' > "${MYDIR}/youpi.bla"
head -c $largecgi /dev/zero | tr '\0' 'Z' > "${MYDIR}/youpi_expected_result.bla"
touch "${MYDIR}/confs/html/uploads_large/bla.bla"
cmd="curl http://$name_server:3490/large_upload/bla.bla"
upfile="youpi.bla"
code="202"
testfile="${MYDIR}/youpi_expected_result.bla"
short_output="true";
compare_size="$largecgi";
fail="true"; # comparing files would get general_tests.sh oom killed.
unittest "Test POST chunked /directory/youpi.bla large file."
rm "${MYDIR}/youpi.bla"
rm "${MYDIR}/youpi_expected_result.bla"
rm "${MYDIR}/confs/html/uploads_large/bla.bla"

###################################################################
###################################################################
###################################################################

{ anounce MULTI_CGI_BLA_FAIL \
\
	'Test CGI call when posting and calling something (UNexistent) .bla.' \
\
; } 2> /dev/null

echo -n "This file is exactly 99 bytes long, and is used to test POST requests. This text is printable: EOF!" > ${MYDIR}/99B.words
cmd="curl http://$name_server:3490/xxxx.bla";
upfile="99B.words" 
code="421";
show_output="true";
message="Arbitrary choice of 421. POST using CGI on bad URL."
unittest "Error calling CGI multipart with wrong file";
rm ${MYDIR}/99B.words

###################################################################

{ anounce CGI_CHUNK_UNEXISTENT \
\
	'Unexistent .bla file.' \
\
; } 2> /dev/null

chunked="true"
echo -n "This file is exactly 99 bytes long, and is used to test POST requests. This text is printable: EOF!" > ${MYDIR}/99B.words
cmd="curl http://$name_server:3490/xxxx.bla";
upfile="99B.words" 
code="421";
show_output="true";
message="Arbitrary choice of 421. POST using CGI on bad URL."
unittest "Error calling CGI chunked with wrong file";
rm ${MYDIR}/99B.words

#################################################################
##################################################################
##################################################################

{ anounce Custom_Error \
\
	':3490 has custom error.' \
\
; } 2> /dev/null

cmd="curl http://$name_server:3490/x"
code="404"
testfile="$MYDIR/confs/html/40x.html";
show_output="true"
unittest "Error 404 with custom configuration"

#################################################################

{ anounce Custom_Error \
\
	':3491 does not have custom.' \
\
; } 2> /dev/null

cmd="curl http://$name_server:3491/x"
code="404"
testfile="$MYDIR/../default_responses/404.html";
show_output="true"
unittest "Error 404 without custom"

#################################################################

{ anounce Directory_Listing \
	'autoindex-demo' \
; } 2> /dev/null

cmd="curl http://$name_server:3490/autoindex-demo"
code="200"
show_output="true"
message="Must receive HTML of directory listing."
unittest "Autoindex"

#################################################################

{ anounce Directory_Listing_OFF \
	'autoindex-off, existent directory, expect 403' \
; } 2> /dev/null

cmd="curl http://$name_server:3490/autoindex-off"
code="403"
show_output="true"
unittest "Autoindex off"

#################################################################

{ anounce ReRoot_Autoindex \
	'This shows files that were previously uploaded \n
	 to large_uploads (root uploads_large).' \
; } 2> /dev/null

cmd="curl http://$name_server:3490/large_uploads"
code="200"
show_output="true"
unittest "Another autoindex, now with root rewrite"

#################################################################
#################################################################
#################################################################

{ anounce DELETE_HARDFILE \
	'This deletes a file that was put by the system.' \
; } 2> /dev/null

before="`ls ${MYDIR}/confs/html/uploads_large`";
echo 'This file will be deleted.' > "${MYDIR}/confs/html/uploads_large/file_to_be_deleted";
during="`ls ${MYDIR}/confs/html/uploads_large`";
run "curl -X DELETE http://$name_server:3490/large_upload/file_to_be_deleted -sSvw '%{http_code}' -o foo_out";
after="`ls ${MYDIR}/confs/html/uploads_large`";
say "ls before DELETE: $before";
say "ls during DELETE: $during";
say "ls after DELETE: $after";
say "Got body: `cat foo_out`";
rm foo_out;
colorscore "ls must be the same after DELETE call" "$before" "$after";
colorscore "Expect 200 OK if has body, 204 No Content if response has no body" "$out" "200";

#################################################################

{ anounce DELETE_A_POST \
	'This POSTs a file, then DELETEs it.' \
; } 2> /dev/null

before="`ls ${MYDIR}/confs/html/uploads_large`";
head -c 142 /dev/urandom > ${MYDIR}/noise_to_delete
run "curl http://$name_server:3490/large_uploads -F 'file=@${MYDIR}/noise_to_delete'"
during="`ls ${MYDIR}/confs/html/uploads_large`";
run "curl -X DELETE http://$name_server:3490/large_upload/noise_to_delete -sSvw '%{http_code}' -o foo_out";
after="`ls ${MYDIR}/confs/html/uploads_large`";
say "ls before DELETE: $before";
say "ls during DELETE: $during";
say "ls after DELETE: $after";
say "Got body: `cat foo_out`";
rm foo_out;
colorscore "ls must be the same after DELETE call" "$before" "$after";
colorscore "Expect 200 OK if has body, 204 No Content if response has no body" "$out" "200";
rm ${MYDIR}/noise_to_delete

#################################################################

{ anounce DELETE_A_DIRECTORY \
	'Tries to delete a directory that has DELETE permission. \n
	 Forcefully implemented 403 Forbidden for all cases.' \
; } 2> /dev/null

run "curl -X DELETE http://$name_server:3490/large_upload -sSvw '%{http_code}' -o foo_out";
say "Got body: `cat foo_out`";
rm foo_out;
colorscore "Expect 403" "$out" "403"

#################################################################

{ anounce DELETE_UNEXISTENT_FILE \
	'Try to delete an unexistent file.' \
; } 2> /dev/null

run "curl -X DELETE http://$name_server:3490/large_upload/unexistent_file -sSvw '%{http_code}' -o foo_out";
say "Got body: `cat foo_out`";
rm foo_out;
colorscore "Expect 404" "$out" "404"

##################################################################
#################################################################
#################################################################

{ anounce COOKIE \
\
	"Show cookie usage and session management." \
\
; } 2> /dev/null

set +x
run "curl http://$name_server:3490/cgi_cookie.php -b cookiefile -c cookiefile --trace-ascii tmp_response"
cookieline=`cat tmp_response | grep "Set-Cookie" | sed "s|.*Set-Cookie.*|Set-Cookie|"`
sessionid=`cat tmp_response | grep "Set-Cookie" | sed "s|.*SESSION_ID=||" | sed "s|;.*||"`
dog tmp_response;
colorscore "Must have gotten some Set-Cookie header." "$cookieline" "Set-Cookie";
sleep 2
curl http://$name_server:3490/cgi_cookie.php -b cookiefile -c cookiefile -so tmp_response;
cookieline=`cat tmp_response | grep "$sessionid"`;
dog tmp_response;
colorscore "SESSION_ID must be set." "$cookieline" "$sessionid";
sleep 4
curl http://$name_server:3490/cgi_cookie.php -b cookiefile -c cookiefile -so tmp_response;
cookieline=`cat tmp_response | grep "$sessionid"`;
dog tmp_response;
colorscore "SESSION_ID expired." "$cookieline" "";
rm tmp_response;
rm cookiefile;

#################################################################
##################################################################
##################################################################

{ anounce STRESS_GET \
\
	"Stress testing $stress_count calls. Wait for it.\n
	This can be changed on general_tests.sh:stress_count" \
\
; } 2> /dev/null

stress_count=142;

set +x;
rm -f stress_out;
[ "$silent" = "" ] && [ "$ultrasilent" = "" ] && echo "";
i=1;
while [ "$i" -le "$stress_count" ]; do
	[ "$ultrasilent" = "" ] && echo -n "\r $i";
	curl -sv http://localhost:3491/ 2>> stress_out 1> /dev/null &
	i=$(( i + 1 ));
done;
wait;
stress_result=$(cat stress_out | grep HTTP | grep "200 OK" | wc -l);
colorscore "\rCount of 200 OK must be $stress_count, and it is $stress_result" "$stress_count" "$stress_result";
rm -f stress_out;

##################################################################

stress_count=100;
noise_size="1MB";
more_than="95";
stressupmulti

stress_count=50;
noise_size="2MB";
more_than="45";
stressupmulti

stress_count=20;
noise_size="3MB";
more_than="15";
stressupmulti

stress_count=10;
noise_size="10MB";
more_than="9";
stressupmulti

stress_count=2;
noise_size="50MB";
more_than="1";
stressupmulti

##################################################################

stress_count=100;
noise_size="1MB";
more_than="95";
stressupchunk

stress_count=50;
noise_size="2MB";
more_than="45";
stressupchunk

stress_count=30;
noise_size="3MB";
more_than="25";
stressupchunk

stress_count=10;
noise_size="10MB";
more_than="9";
stressupchunk

stress_count=2;
noise_size="50MB";
more_than="1";
stressupchunk

##################################################################

finish; # < < < < < < < < < < < < < < < < < < < < < End line!

##################################################################
