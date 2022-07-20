#!/bin/sh

echo 'Cleaning uploaded files.'

clean()
{
	rm $1 2> /dev/null;
	ls -l $1 2> /dev/null;
}

clean confs/html/99B.words
clean confs/html/file.noise
#clean confs/html4242/uploads/99B.words

echo 'You should not see any file listed above this line.'
