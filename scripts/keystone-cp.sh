#!/bin/bash -x

PACKAGE_FILES="${TEE_REF_TA_DIR}/ref-ta/keystone/App/app \
		${TEE_REF_TA_DIR}/ref-ta/keystone/Enclave/enclave.eapp_riscv \
		${KEYSTONE_SDK_DIR}/rts/eyrie/eyrie-rt"

CLIENT_APP=app

OUTPUT_DIR=${KEYSTONE_DIR}/buildroot_overlay/root/ref-ta

rm -fr ${OUTPUT_DIR}
mkdir -p ${OUTPUT_DIR}

for output in $PACKAGE_FILES; do
  cp $output $OUTPUT_DIR/.
done

ls -l $OUTPUT_DIR
