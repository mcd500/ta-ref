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
screen -L -dmS qemu /bin/bash -c "${CURDIR}/../${TEE}/scripts/launch-qemu.sh"
# waiting for booting..
sleep 7

source ${CURDIR}/expect.sh ANALYZE=${ANALYZE}

screen -S qemu -X poweroff

# cleanup
pkill screen
