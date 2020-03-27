#!/bin/bash

rm -f ~/.ssh/known_hosts

CURDIR=`dirname $0`
# for using expect command:)
export PORT=${PORT}
export USER=${USER}
export IP_ADDR=${IP_ADDR}
export PASSWD=${PASSWD}
export PS1="$ "

if [ "${ANALYZE}" = "ON" ]; then
${CURDIR}/expect/analyze.expect
else
${CURDIR}/expect/demo.expect
fi
