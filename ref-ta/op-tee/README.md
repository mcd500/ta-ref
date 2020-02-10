# tutorial

## qemu

```
# Dockerfile
# see https://github.com/OP-TEE/build/blob/3.7.0/qemu_v8.mk
FROM knknkn1162/test:qemu_v8
ENV OPTEE_DIR=/home/main/optee
# for tzdata
ENV ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update &&
  apt-get install -y socat expect

RUN git clone http://192.168.100.100/vc707/tee-ta-reference.git -b optee_perf
```

```sh
docker build . -t optee_ta
docker run -it --rm -w /home/main/tee-ta-reference/ref-ta/op-tee optee_ta

make # or make PROFILER=ON
make copyto
make run # or make perf
```

## raspberry pi3

### build tee-ta-reference and fetch rootfs image

```
$ docker run  -it -v $(pwd):/home/main/shared -v ${TEE_TA_REFERENCE_DIR}:/home/main/tee-ta-reference --rm -w /home/main/tee-ta-reference/ref-ta/op-tee knknkn1162/test:rpi3
# inside docker
$ export OPTEE_DIR=/home/main/optee
# If necessary, customize optee_os parameter( e.g) `CFG_TEE_**` ) in ${OPTEE_DIR}/build/common.mk.
$ make && make copyto
# fetch rootfs.cpio.gz
$ cp /home/main/optee/out-br/images/rootfs.cpio.gz /home/main/shared
```

Note) In case of manual build of optee, you should get with the master branch. Unfortunately in the 3.7.0 stable version we failed `xtest` command.


### write image to sd card

```
# no longer use docker container!
# check `lsblk` and umount mountpoints connected with /dev/sde
# assume that device file with SD card is /dev/sde. This instruction is followed by `make img-help`
$ umount /media/$(whoami)/{rootfs,BOOT}
$ sudo fdisk /dev/sde   # where sdx is the name of your sd-card
   > p             # prints partition table
   > d             # repeat until all partitions are deleted
   > <enter>
   > d
   ..
   > n             # create a new partition
   > p             # create primary
   > 1             # make it the first partition
   > <enter>       # use the default sector
   > +32M          # create a boot partition with 32MB of space
   > n             # create rootfs partition
   > p
   > 2
   > <enter>
   > <enter>       # fill the remaining disk, adjust size to fit your needs
   > t             # change partition type
   > 1             # select first partition
   > e             # use type 'e' (FAT16) : W95 FAT16 (LBA)
   > a             # make partition bootable
   > 1             # select first partition
   > p             # double check everything looks right
Command (m for help): p
Disk /dev/sde: 14.5 GiB, 15523119104 bytes, 30318592 sectors
Units: sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disklabel type: dos
Disk identifier: 0x00000000

Device     Boot Start      End  Sectors  Size Id Type
/dev/sde1  *     2048    67583    65536   32M  e W95 FAT16 (LBA)
/dev/sde2       67584 30318591 30251008 14.4G 83 Linux

   > w             # write partition table to disk.
The partition table has been altered.
Calling ioctl() to re-read partition table.
Syncing disks.

# configure partition 1
$ sudo mkfs.vfat -F16 -n BOOT /dev/sde1
$ mkdir -p ./media
$ sudo mount /dev/sde1 ./media
$ cd ./media
$ gunzip -cd ../rootfs.cpio.gz | sudo cpio -idmv "boot/*"
boot/start_db.elf
boot/fixup_db.dat
boot/start_x.elf
boot/fixup.dat
boot/armstub8.bin
boot/LICENCE.broadcom
boot/bcm2710-rpi-3-b.dtb
boot/kernel8.img
boot/COPYING.linux
boot/start.elf
boot/fixup_x.dat
boot/config.txt
boot/bootcode.bin
boot/fixup_cd.dat
boot/start_cd.elf
boot/bcm2710-rpi-3-b-plus.dtb
boot/uboot.env
207937 blocks
# TODO: how to fix the prefious command?
$ sudo mv ./boot/* ./ && sudo rm -r boot
$ cd ../ && sudo umount ./media

# configure partion 2
## take some time..
$ sudo mkfs.ext4 -L rootfs /dev/sde2
$ sudo mount /dev/sde2 ./media
$ cd ./media
$ gunzip -cd ../rootfs.cpio.gz | sudo cpio -idmv
$ sudo rm -rf -v ./boot/*
removed './media/boot/armstub8.bin'
removed './media/boot/bcm2710-rpi-3-b.dtb'
removed './media/boot/bcm2710-rpi-3-b-plus.dtb'
removed './media/boot/bootcode.bin'
removed './media/boot/config.txt'
removed './media/boot/COPYING.linux'
removed './media/boot/fixup_cd.dat'
removed './media/boot/fixup.dat'
removed './media/boot/fixup_db.dat'
removed './media/boot/fixup_x.dat'
removed './media/boot/kernel8.img'
removed './media/boot/LICENCE.broadcom'
removed './media/boot/start_cd.elf'
removed './media/boot/start_db.elf'
removed './media/boot/start.elf'
removed './media/boot/start_x.elf'
removed './media/boot/uboot.env'
$ cd ../ && sudo umount ./media
# make sure that SD card is unmounted by `lsblk` or `mount -v`.
```

### check xtest command on buildroot environment

See Step.7 ~ Step.9 on https://optee.readthedocs.io/en/latest/building/gits/build.html

```
## insert SD card and launch raspberry pi3
# login: root
# which xtest
# xtest
** subsets of which 0 failed
** test cases of which 0 failed
0 test cases were skipped
TEE test application done!
```

### create debian(stretch: 9) environment

From this section, we configure debian(stretch) chroot environment and run xtest program on it.

```sh
# insert SD card reader
$ lsblk
sde           8:64   1  14.5G  0 disk
├─sde1        8:65   1    32M  0 part
└─sde2        8:66   1  14.4G  0 part
$ umount /media/$(whoami)/{rootfs,BOOT}
$ mkdir -p ./mnt
$ sudo mount /dev/sde2 ./mnt && cd ./mnt
# create debian environment for chroot
$ export ROOTFS=debian_rootfs
$ sudo mkdir -p ${ROOTFS}
$ sudo debootstrap --arch=arm64 --foreign --include=ifupdown,netbase,iproute,openssh-server,file,sudo,less stretch ./${ROOTFS}
$ sudo cp `which qemu-aarch64-static` ./${ROOTFS}/usr/bin
$ sudo chroot ./${ROOTFS} debootstrap/debootstrap --second-stage
```

Then, prepare the script(we call `debian_chroot.sh`) to dive into debian environment easily. set './root' directory:

```sh
ROOTFS=/debian_rootfs
mount --bind /dev ${ROOTFS}/dev
mount -t devpts devpts ${ROOTFS}/dev/pts
mount --bind /sys ${ROOTFS}/sys
mount --bind /proc ${ROOTFS}/proc
chroot ${ROOTFS} /bin/bash
```

### copy tee_ref_ta and related files

Then copy files from tee-te-reference;

+ usr/lib/{libteec.so.1.0.0, libteec.so.1}
+ /lib/optee_armtz/$(BINARY).ta[a6f77c1e-96fe-4a0e-9e74-262582a4c8f1.ta]
+ analyzer(program)
+ enclave_nm
+ optee_ref_ta(program)

```sh
# in the mountpoint
sudo rsync -av ./root/ ./debian_rootfs/root/
sudo rsync -av ./usr/bin/optee_* ./debian_rootfs/usr/bin/
sudo rsync -av ./lib/optee_armtz/ ./debian_rootfs/lib/optee_armtz/

sudo rsync -av ./usr/lib/libteec.so.1.0.0 ./debian_rootfs/usr/lib/
sudo ln -s libteec.so.1.0.0 ./debian_rootfs/usr/lib/libteec.so.1
cd ../ && sudo umount ./mnt
```

Note) We don't have to use `LD_PRELOAD` to adjust the library version. All we have to do is libteec into the debian chroot environment

### xtest in debian chroot environment

Insert the SD card in the raspberry pi board and poweron:

```sh
# login and make sure that we are in /root directory.
# buildroot environment
./debian_chroot.sh
# dive into debian chroot environment
cd root & optee_ref_ta
# TODO: where should we keep the log?
./analyzer shared_mem enclave_nm
```
