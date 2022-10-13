<?php

$session_id = getenv("WS_SESSION_ID");
if (!$session_id)
{
	echo "Set-Cookie: SESSION_ID=unique_session_id; Max-Age=5";
	echo "\n\n";
}

echo "||| CGI-COOKIE ||| ";
if ($session_id)
{
	echo "Cookie defined session id -> ";
	echo "\n";
	echo $session_id;
	echo "\n";
	echo " <-";
}
else
{
	echo "Cookie has no ID. No session running.";
	echo "\n";
	echo "This script now sets a cookie for 5 seconds session.";
}
echo "\n";

?>
