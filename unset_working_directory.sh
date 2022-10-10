#!/bin/sh

MYSELF="$(realpath "$0")"
MYDIR="${MYSELF%/*}"

sed -i "s|${MYDIR}|WORKING_DIRECTORY|" ${MYDIR}/webserv-default.conf
sed -i "s|${MYDIR}|WORKING_DIRECTORY|" ${MYDIR}/unit/webserv-unit.conf
