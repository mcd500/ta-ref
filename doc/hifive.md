# How to run keystone on hifive board

# Reference

+ http://docs.keystone-enclave.org/en/latest/Getting-Started/Running-Keystone-on-Hardware.html

+ http://192.168.100.100/rinkai/keystone-docs/-/blob/master/hifive-keystone-build.md

# build image

The below files should be prepared:

|file|docker image|remarks|partition|
|---|---|---|
|bbl.bin|[vc707/test:keystone_old](http://192.168.100.100/rinkai/dockerfiles/-/blob/master/keystone/old/Dockerfile)|risc-pk(sm) + linux kernel|1st partition|
|rootfs|[]()|debian image(including keystone-driver)|2nd partition|
|fsbl.bin|[vc707/test:hifive_bootloader](http://192.168.100.100/rinkai/dockerfiles/-/blob/master/riscv_toolchain_qemu/hifive_bootloader/Dockerfile)|1st stage bootloader|4th partition|

Note) Keystone v0.4 image could not get the linux kernel running even if we specify `cmake .. -Dsifive=y` in build step). We suggest that the old version of [keystone-sample-image](http://docs.keystone-enclave.org/en/latest/Getting-Started/Running-Keystone-with-QEMU.html) should be used.

If we want to develop keystone, we should build with `make image`:

```sh
docker run -it --rm vc707/test:keystone_old
# customize riscv-pk(sm) or keystone-driver
make image
# ${KEYSTONE_DIR}/hifive-work/bbl.bin file is built
```

# write image to sd card

## create partition

See [the link](http://docs.keystone-enclave.org/en/latest/Getting-Started/Running-Keystone-on-Hardware.html) or http://192.168.100.100/rinkai/keystone-docs/-/blob/master/hifive-keystone-build.md .

## write data to sd card

```sh
# assume that the device file in sd card is /dev/sdb.
sudo dd if=bbl.bin of=/dev/sdb1 bs=4096 conv=fsync
# write rootfs on /dev/sdb2
sudo dd if=fsbl.bin of=/dev/sdb4 bs=4096 conv=fsync
```

# run keystone on hifive

```sh
# check that MSEL2 dip is switched off (outward position).
# UART0(MicroUSB) connector is inserted
# power on
sudo screen /dev/ttyUSB1 115200
```
