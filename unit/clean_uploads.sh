#!/bin/bash

MYSELF="$(realpath "$0")"
MYDIR="${MYSELF%/*}"

echo 'Cleaning uploaded files.'
echo -e "------------------------\033[0;31m"

clean()
{
	rm $1 2> /dev/null;
	ls -l $1 2> /dev/null;
}

clean ${MYDIR}/confs/html/99B.words
clean ${MYDIR}/confs/html/file.noise
clean ${MYDIR}/confs/html/uploads_large/file.noise
clean ${MYDIR}/confs/html/99B.bla
clean ${MYDIR}/file.noise
clean ${MYDIR}/99B.words

find ${MYDIR} | grep \\.noise;
find ${MYDIR} | grep \\.words;
find ${MYDIR} | grep 99B.bla;

echo -e "\033[0;37m------------------------"
echo 'You should not see any file listed above this line.'
echo -e "\033[0;37m------------------------\n\n"
