<?php

$session_id = getenv("WS_SESSION_ID");
if (!$session_id)
{
	echo "Set-Cookie: SESSION_ID=foo; Max-Age=5";
	echo "\r\n\r\n";
}

echo "||| CGI-COOKIE ||| ";
if ($session_id)
{
	echo "Cookie defined session id -> ";
	echo $session_id;
	echo " <-";
}
else
{
	echo "Cookie has no ID. No session running.";
}
echo "\r\n";

?>
