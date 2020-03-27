#!/bin/bash
CURDIR=`dirname $0`
echo $DIR
# That would display enclave outputs!
screen -L -dmS qemu /bin/bash -c "PORT=${PORT} ${CURDIR}/run-qemu.sh"
# waiting for booting..
sleep 7

if [ "${ANALYZE}" = "ON" ]; then
${CURDIR}/expect/analyze.sh
else
${CURDIR}/expect/demo.sh
fi
