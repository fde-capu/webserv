#!/bin/bash
echo "first-line";
echo "DUMB_CGI!"
echo "---->FULL ARGS: $@"

#echo '---->ARGS:'
# for i; do
#    echo $i
# done

#echo '---->ENV'
#env

echo '---->READ:'
#while

IFS= read -r line;
echo "$line";

#done < /dev/stdin
