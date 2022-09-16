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
clean ${MYDIR}/confs/html4242/uploads/99B.words
clean ${MYDIR}/confs/html4242/uploads/file.noise
clean ${MYDIR}/confs/html/uploads_large/file.noise
clean ${MYDIR}/confs/html/99B.bla
clean ${MYDIR}/file.noise
clean ${MYDIR}/99B.words

find ${MYDIR} | grep noise;
find ${MYDIR} | grep words;
find ${MYDIR} | grep 99B.bla;

echo -e "\033[0;37m------------------------"
echo 'You should not see any file listed above this line.'

echo 'Initial YoupiBanane/youpi.bla' > ${MYDIR}/confs/html4242/YoupiBanane/youpi.bla
ls -l ${MYDIR}/confs/html4242/YoupiBanane/youpi.bla

echo 'Above should be confs/html4242/YoupiBanane/youpi.bla as 30B.'

