#!/bin/bash

rm -f ~/.ssh/known_hosts

CURDIR=`dirname $0`
# for using expect command:)
export PORT=${PORT}
export USER=${USER}
export IP_ADDR=${IP_ADDR}
export PASSWD=${PASSWD}

# TODO: change PS1 variable on remote host
export PS1="# "

# That would display enclave outputs!
screen -L -dmS qemu /bin/bash -c "${CURDIR}/launch-qemu.sh"
# waiting for booting..
sleep 7

if [ "${ANALYZE}" = "ON" ]; then
${CURDIR}/expect/analyze.expect
else
${CURDIR}/expect/demo.expect
fi

screen -S qemu -X poweroff
