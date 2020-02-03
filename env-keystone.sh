if [ -z "$GITLAB_HOST" ]; then
  export GITLAB_HOST=192.168.100.100
fi
export KEYSTONE_DIR=`pwd`/build-keystone
export KEYSTONE_SDK_DIR=${KEYSTONE_DIR}/sdk
export OPTEE_DIR=`pwd`/build-optee
export PATH=${PATH}:${KEYSTONE_DIR}/riscv/bin:${OPTEE_DIR}/toolchains/aarch64/bin
