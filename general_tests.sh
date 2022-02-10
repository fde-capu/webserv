#!/bin/bash

set -x

divider()
{
	{ set +x; } 2> /dev/null
	echo -e '\n\n--------------------------\n\n'
	set -x
}

{ divider; } 2> /dev/null
{ divider; } 2> /dev/null
{ divider; } 2> /dev/null

curl -vD- localhost:3490

{ divider; } 2> /dev/null

curl -vD- localhost:3491

{ divider; } 2> /dev/null

curl -vD- localhost:3492

{ divider; } 2> /dev/null

curl -vD- localhost:3493

{ divider; } 2> /dev/null
