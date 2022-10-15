#!/bin/sh

MYSELF="$(realpath "$0")"
MYDIR="${MYSELF%/*}"

sed -i "s|^working_directory.*|working_directory WORKING_DIRECTORY/unit/confs|" ${MYDIR}/webserv-default.conf
sed -i "s|^working_directory.*|working_directory WORKING_DIRECTORY/unit/confs|" ${MYDIR}/unit/webserv-unit.conf
