# raspberry pi3 tutorial

In this chapter, we configure image based on [3.8.0](https://github.com/OP-TEE/manifest/tree/3.8.0).

## build tee-ta-reference and fetch rootfs image

```sh
$ docker run  -it -v $(pwd):/home/out -v ${pwd}:/home/main/ta-ref --rm -w /home/main/ta-ref trasioteam/optee:rpi3_3.9.0
# inside docker. import rootfs.cpio.gz from the container to local
$ cp /home/main/optee/out-br/images/rootfs.cpio.gz /home/out
```

## write image to sd card

Write the above image to sd card according to `make img-help` @ optee_build:

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

## create debian(buster: 10) environment

From this section, we configure debian(buster) for chroot environment.

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
# select debian-buster to match libc version between buildroot and debian
# see https://packages.debian.org/ja/buster/libc6-dev
$ sudo debootstrap --arch=arm64 --foreign --include=libc6-dev,ifupdown,netbase,iproute2,openssh-server,file,sudo,less,vim buster ./${ROOTFS}
$ sudo cp `which qemu-aarch64-static` ./${ROOTFS}/usr/bin
$ sudo chroot ./${ROOTFS} debootstrap/debootstrap --second-stage
```

Then, prepare the script(we call `debian_chroot.sh`) to dive into debian environment easily. set './root' directory:

```sh
ROOTFS=/debian_rootfs
mount --bind /dev ${ROOTFS}/dev
# To avoid the sudo error, `sudo: no tty present and no askpass program specified`
mount -t devpts devpts ${ROOTFS}/dev/pts
mount --bind /sys ${ROOTFS}/sys
mount --bind /proc ${ROOTFS}/proc
chroot ${ROOTFS} /bin/bash
```

### copy tee_ref_ta and related files

Then copy files from base image to chroot environment;

+ usr/lib/{libteec.so.1.0.0, libteec.so.1}
+ usr/sbin/tee-supplicant

```sh
# in the mountpoint
sudo cp -a ./usr/sbin/tee-supplicant ./${ROOTFS}/usr/sbin/
sudo cp -a ./usr/lib/libteec.so.1.0.0 ./${ROOTFS}/usr/lib/
sudo ln -s libteec.so.1.0.0 ./${ROOTFS}/usr/lib/libteec.so.1
mkdir -p ./${ROOTFS}/lib/optee_armtz/
cd ../ && sudo umount ./mnt
```

## TEE api test in raspberry pi3

### Launch ssh daemon

Insert the SD card in the raspberry pi board and poweron. exec ssh and tee-supplicant daemon **in chroot environment**:

```sh
# login and make sure that we are in /root directory.
# buildroot environment
# disable tee-supplicant
ps aux | grep tee-supp
kill -9 ${SUPP_PROCESS_NUM}
# chroot
./debian_chroot.sh
```

```sh
# dive into debian chroot environment
# enable tee-supplicant
tee-supplicant -d
dhclient
/etc/init.d/ssh restart
# optional settings
adduser gitlab
EDITOR=vim visudo
 # Allow members of group sudo to execute any command
-%sudo  ALL=(ALL:ALL) ALL
+%sudo  ALL=(ALL:ALL) NOPASSWD:ALL

adduser gitlab # set password to 'gitlab'
usermod -aG sudo gitlab
sudo sh -c 'echo 127.0.1.1 buildroot >> /etc/hosts'
```

If necessary, We can add user and configure `sudoers` file.

### Test TEE and GP APIs

Finally, test TEE API and `gp_**` function. See http://192.168.100.100/vc707/ta-ref#raspberry-pi3:

```sh
git clone --recursive http://192.168.100.100/vc707/ta-ref.git && cd ta-ref
docker run -it --rm -v $(pwd):/home/main/ta-ref vc707/test:ta_ref_optee_rpi3_devel
# inside docker
cd ta-ref
source env/optee_rpi3.sh
make select
make build PROFILER=ON
# scp from local to raspberry pi3.
make test PROFILER=ON
# run test including profiler
make run
```
