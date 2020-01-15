#!/bin/bash -x

PACKAGE_FILES=" \
		${TEE_REF_TA_DIR}/ref-ta/keystone/App/App.client \
		${TEE_REF_TA_DIR}/ref-ta/keystone/Enclave/Enclave.eapp_riscv \
		${KEYSTONE_SDK_DIR}/rts/eyrie/eyrie-rt"

OUTPUT_DIR=${KEYSTONE_DIR}/buildroot_overlay/root/ref-ta
OUTPUT1_DIR=${KEYSTONE_DIR}/hifive-work/buildroot_initramfs/target/root/ref-ta

rm -fr ${OUTPUT_DIR} || true
rm -fr ${OUTPUT1_DIR} || true
mkdir -p ${OUTPUT_DIR}
mkdir -p ${OUTPUT1_DIR}

for output in $PACKAGE_FILES; do
  cp -a $output $OUTPUT_DIR/. || exit 1
  cp -a $output $OUTPUT1_DIR/. || exit 1
done

ls -l $OUTPUT_DIR
ls -l $OUTPUT1_DIR
