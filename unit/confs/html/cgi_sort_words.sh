#!/bin/sh

echo "SORTED WORDS";
IFS= read something_from_stdin;
echo "Read from stdin: $something_from_stdin"
sorted=`echo "$something_from_stdin" | tr ' ' '\n' | sort | tr '\n' ' '`
echo "Sorted words:    $sorted"
