#!/bin/bash
echo 'DUMB_CGI!'
#echo '@'
#echo $@;
#echo 'ARGV'
# for i; do
#    echo $i
# done
#echo 'ENV:'
#env
echo 'RAW:'
while IFS= read -r line ; do
    printf "%s\n" "$line"
done < /dev/stdin
