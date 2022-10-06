#!/bin/sh

MYSELF="$(realpath "$0")"
MYDIR="${MYSELF%/*}"

sed -i "s|WORKING_DIRECTORY|${MYDIR}|" ${MYDIR}/webserv-default.conf
sed -i "s|WORKING_DIRECTORY|${MYDIR}|" ${MYDIR}/unit/webserv-unit.conf
