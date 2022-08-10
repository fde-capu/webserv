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
clean ${MYDIR}/confs/html4242/uploads/99B.words
clean ${MYDIR}/confs/html4242/uploads/file.noise

echo 'You should not see any file listed above this line.'

echo 'Initial YoupiBanane/youpi.bla' > ${MYDIR}/confs/html4242/YoupiBanane/youpi.bla
ls -l ${MYDIR}/confs/html4242/YoupiBanane/youpi.bla

echo 'Abouve should be confs/html4242/YoupiBanane/youpi.bla as 30B.'

