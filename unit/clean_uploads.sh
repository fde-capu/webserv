#!/bin/bash

MYSELF="$(realpath "$0")"
MYDIR="${MYSELF%/*}"

echo 'Cleaning uploaded files.'

clean()
{
	rm $1 2> /dev/null;
	ls -l $1 2> /dev/null;
}

clean ${MYDIR}/confs/html/99B.words
clean ${MYDIR}/confs/html/file.noise
#clean confs/html4242/uploads/99B.words

echo 'You should not see any file listed above this line.'
