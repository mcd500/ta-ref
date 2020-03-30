#!/bin/bash

rm -f ~/.ssh/known_hosts

CURDIR=`dirname $0`
# for using expect command:)
export PORT=${PORT}
export USER=${USER}
export IP_ADDR=${IP_ADDR}
export PASSWD=${PASSWD}
export PS1="$ "

source ${CURDIR}/expect.sh ANALYZE=${ANALYZE}
