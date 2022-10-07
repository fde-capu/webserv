#!/bin/sh

# unit test for webserv
# by fde-capu

# All variables are true if some string "anything" and false as empty string "".

name_server="127.0.0.1";
step_by_step="true";
clean_upfiles_after_test="";

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
	message="";
}
resetvars;

unittest()
{
	fullcmd="$cmd";

	if [ "$noise" != "" ] ; then
		upfile="file.noise";
		head -c $noise /dev/urandom > "${MYDIR}/$upfile";
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
	fi

	[ "$trace" != "" ] && fullcmd="$fullcmd --trace-ascii tmp_trace_ascii";
	fullcmd="$fullcmd -o tmp_response";
	
	out=`eval "$fullcmd"`;

	[ "$show_output" != "" ] && echo "<<<" && cat tmp_response && echo "<<<";

	if [ "$out" = "000" ]; then
		{ anounce ERROR 'Make sure the server is running! (Response 0?)'; } 2> /dev/null;
		exit 1;
	fi

	[ "$fail" = "" ] && [ "$testfile" != "" ] && echo "File comparison: $testfile";

	colorscore "$1 | expect $code, got $out" "$out" "$code"

	if [ "$fail" = "" ] ; then
		[ "$testfile" != "" ] && colorscore "$1 | compare ouput" "`cat tmp_response`" "`cat $testfile`";
		[ "$outdir" != "" ] && [ "$upfile" != "" ] && colorscore "$1 | compare files" "`cat $outdir/$upfile`" "`cat ${MYDIR}/$upfile`";
	fi

	[ "$trace" != "" ] && cat tmp_trace_ascii

	[ "$message" != "" ] && echo "\033[0;33m$message\033[0;37m";

	[ "$clean_upfiles_after_test" != "" ] && [ "$noise" != "" ] && rm ${MYDIR}/$upfile;
	[ "$clean_upfiles_after_test" != "" ] && rm tmp_response;
	[ "$clean_upfiles_after_test" != "" ] && [ "$trace" != "" ] && rm tmp_trace_ascii

	resetvars;
}

enterkey()
{
	[ "$step_by_step" = "" ] && return;
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
	{ ${MYDIR}/clean_uploads.sh; }
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

colorscore()
{
	a=`echo "$2"`;
	b=`echo "$3"`;
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
show_output="true";
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
show_output="true";
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
show_output="true";
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
show_output="true";
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
show_output="true";
unittest "Port to server";

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

cmd="curl -L http://$name_server:3493"
testfile="$MYDIR/confs/html/index.htm";
code="200";
unittest "Client redirecting made two calls";

##################################################################
##################################################################
##################################################################

{ anounce Not_this_directory \
\
	'Forbid existent server_name without root definition. 403' \
\
; } 2> /dev/null

cmd="curl http://$name_server:3490 -H 'Host: rootless'";
code="403";
unittest "server_name w/o root (forbidden)";

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
noise="1MB";
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
unittest "Forbidden";

##################################################################
##################################################################
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
unittest "Simple post";
ls -l ${MYDIR}/confs/html/99B.words;
rm ${MYDIR}/99B.words
cat ${MYDIR}/confs/html/99B.words;

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
unittest "Simple post with noise";

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
unittest "Post noise 100B";

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
unittest "Post noise 101B";

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
show_output="true"
unittest "Noise to large_upload 42B"

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

#####################################################################

{ anounce MULTI_50M_NOISE \
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

{ anounce MULTI_FAIL \
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
unittest "webserv must close connection"

#####################################################################

{ anounce MULTI_FAIL \
\
	'How about 200MB? Wait a little, but this would get OOM KILL\n
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

###################################################################
###################################################################
###################################################################

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
ls -l ${MYDIR}/confs/html/99B.words;
rm ${MYDIR}/99B.words
cat ${MYDIR}/confs/html/99B.words;

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
unittest "Post noise 100B";

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
unittest "Post noise 101B";

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
unittest "50MB success"

#####################################################################

{ anounce CHUNK_PARTIAL \
\
	'How about 200MB? This time, it is accepted as partial upload, \n
	since curl is set to close and webserv must always close.\n
	Note: takes long. You can manually check unit/confs/html/uploads_large' \
\
; } 2> /dev/null

chunked="true";
noise="200MB"
cmd="curl http://$name_server:3490/large_upload"
code="201"
fail="true"
unittest "Accepts, though incomplete"

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

{ anounce CGI_GET_NOTFOUND \
\
	'Test CGI with GET method, xxxx.sh (unexistent).' \
\
; } 2> /dev/null;

cmd="curl http://$name_server:3490/xxxx.sh"
code="404";
unittest "Get cgi sh but no";

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
unittest "Get cgi uri_alias/hi.sh";

##################################################################

{ anounce CGI_GET_PHP \
\
	'Test CGI with GET method, cgi_test.php.' \
\
; } 2> /dev/null;

cmd="curl http://$name_server:3490/cgi_test.php"
code="200";
show_output="true";
message="There should the output of a script, not the script itself.";
unittest "Get cgi php";

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

{ anounce CGI_CHUNK \
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
unittest "Error 404 width custom configuration"

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
unittest "Error 404"

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
out=`curl -X DELETE http://$name_server:3490/large_upload/file_to_be_deleted -sSvw '%{http_code}' -o foo_out`;
after="`ls ${MYDIR}/confs/html/uploads_large`";
echo "ls before DELETE: $before";
echo "ls after DELETE: $after";
echo "Got body: `cat foo_out`";
rm foo_out;
colorscore "ls must be the same after DELETE call" $before $after;
colorscore "Expect 200 OK if has body, 204 No Content if response has no body" "$out" "200";

#################################################################

{ anounce DELETE_A_POST \
	'This POSTs a file, then DELETEs it.' \
; } 2> /dev/null

before="`ls ${MYDIR}/confs/html/uploads_large`";
head -c 142 /dev/urandom > ${MYDIR}/noise_to_delete
curl http://$name_server:3490/large_uploads -F "file=@${MYDIR}/noise_to_delete"
during="`ls ${MYDIR}/confs/html/uploads_large`";
out=`curl -X DELETE http://$name_server:3490/large_upload/noise_to_delete -sSvw '%{http_code}' -o foo_out`;
after="`ls ${MYDIR}/confs/html/uploads_large`";
echo "ls before DELETE: $before";
echo "ls during DELETE: $during";
echo "ls after DELETE: $after";
echo "Got body: `cat foo_out`";
rm foo_out;
colorscore "ls must be the same after DELETE call" $before $after;
colorscore "Expect 200 OK if has body, 204 No Content if response has no body" "$out" "200";

#################################################################

{ anounce DELETE_A_DIRECTORY \
	'Tries to delete a directory that has DELETE permission. \n
	 Forcefully implemented 403 Forbidden for all cases.' \
; } 2> /dev/null

out=`curl -X DELETE http://$name_server:3490/large_upload -sSvw '%{http_code}' -o foo_out`;
echo "Got body: `cat foo_out`";
rm foo_out;
colorscore "Expect 403" "$out" "403"

#################################################################

{ anounce DELETE_UNEXISTENT_FILE \
	'Try to delete an unexistent file.' \
; } 2> /dev/null

out=`curl -X DELETE http://$name_server:3490/large_upload/unexistent_file -sSvw '%{http_code}' -o foo_out`;
echo "Got body: `cat foo_out`";
rm foo_out;
colorscore "Expect 404" "$out" "404"

#################################################################
##################################################################
##################################################################

{ anounce Stress \
\
	"Stress testing $stress_count calls. Wait for it.\n
	This can be changed on general_tests.sh:stress_count" \
\
; } 2> /dev/null

set +x;

stress_count=100;
rm -f stress_out;
i=1;
while [ "$i" -le "$stress_count" ]; do
	echo -n "\r $i";
	curl -sv http://localhost:3491/ 2>> stress_out 1> /dev/null &
	i=$(( i + 1 ));
done;
wait;
stress_result=$(cat stress_out | grep HTTP | grep "200 OK" | wc -l);
colorscore "\rCount of 200 OK repsponses must be $stress_count, and it is $stress_result" "$stress_count" "$stress_result";
rm -f stress_out;
set -x;

##################################################################
#################################################################
#################################################################

finish; # < < < < < < < < < < < < < < < < < < < < < < < < < < End line!
