#!/bin/bash

export HOST_PORT=${HOST_PORT:="$((3000 + RANDOM % 3000))"}
export TEST_LOG=ref-ta-keystone.log

if [ -f ${KEYSTONE_DIR}/${TEST_LOG} ]; then 
  rm ${KEYSTONE_DIR}/${TEST_LOG}
fi

# Launch QEMU test
screen -L -dmS qemu ${TEE_REF_TA_DIR}/scripts/keystone-run-qemu.sh
sleep 10
${TEE_REF_TA_DIR}/scripts/keystone-match.expect

sed -i "s/.*\[SE\]\ gettimeofday.*/TIME/" ${KEYSTONE_DIR}/${TEST_LOG}
sed -i "s/^REE\ time.*/TIME/" ${KEYSTONE_DIR}/${TEST_LOG}
sed -i "s/.*based\ on\ rdcycle.*/TIME/" ${KEYSTONE_DIR}/${TEST_LOG}

diff -up ${KEYSTONE_DIR}/${TEST_LOG} ${TEE_REF_TA_DIR}/tests/test-qemu.expected.log
if [ $? -eq 0 ]
then
  echo "[PASS] ${TEST_LOG} matches with the expected output"
  exit 0
else
  echo "[FAIL] ${TEST_LOG} does not match with the expected output"
  exit 1
fi

