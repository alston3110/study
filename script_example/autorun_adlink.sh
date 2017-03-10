#! /bin/sh

CLEANSET="true"

SYSPATH="/data"
DATAPATH="/data/flash"
DEVNODE="/dev/mmcblk0"
VERSION="rtx-adlink-yocto-4.15.1-si-test"

# ==================================
MBR_SIZE=64
ROOTFS_SIZE=3379

# ==================================
UBOOT="$DATAPATH/u-boot.imx"
KERNEL="$DATAPATH/uImage.bin"
KERNELDTB="$DATAPATH/uImage.dtb"
RECOVERYKERNEL="$SYSPATH/uImage-recovery"
RECOVERYKERNELDTB="$SYSPATH/recovery.dtb"
RECOVERYRAMDISK="$SYSPATH/uramdisk-recovery.img"
ROOTFS="$DATAPATH/rootfs.tar.bz2"
MD5SUM="$DATAPATH/md5sum.txt"
EFM32CMD="$DATAPATH/efm32cmd"
LIBS="$DATAPATH/lib.tar.bz2"
CHECKFILE_LIST="$UBOOT $KERNEL $KERNELDTB $RECOVERYKERNEL $RECOVERYKERNELDTB $RECOVERYRAMDISK $ROOTFS $EFM32CMD $MD5SUM"

# ==================================

print() {
	echo $1 > /dev/ttymxc1
	echo $1 > /dev/tty1
}

run_end() {
	cd ${SYSPATH}
	print "update Fail!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
	exit 0
}

umount_sd()
{
	mounted=$(mount | grep $DEVNODE | awk '{print $1}' | wc -l)
    if [ $mounted -gt 0 ]; then
		todo=$(mount | grep $DEVNODE | awk '{print $1}')
        for i in $todo
			do
                echo $i
				umount $i
                sleep 1
			done
	fi
}

run() {
	print ""
	print "update system start........."
	/data/efm32cmd 2 12 -sg 0
	cd $DATAPATH
	
	# check file is exist
	md5sum -c md5sum.txt
	if [ $? -ne 0 ]
	then
		run_end
	fi
	
	for CHECKITEM in $CHECKFILE_LIST 
	do
		if [ ! -f $CHECKITEM ]
		then
			run_end
		fi
	done

	print "update efm firmware"
	# update efm firmware
	$EFM32CMD 2 12 -u
	/data/efm32cmd 2 12 -sg 0

	cd $SYSPATH

	#umount $DEVNODE
	umount_sd

	# cal size
	TOTAL_SIZE=`fdisk -l /dev/mmcblk0 | grep 'Disk /dev/mmcblk0:' | awk '{print $5}'`
	UNITS_SIZE=`fdisk -l /dev/mmcblk0 | grep 'Units = cylinders' | awk '{print $9}'`

	MBR_SIZE=`expr $MBR_SIZE \* 1024 \* 1024`
	ROOTFS_SIZE=`expr $ROOTFS_SIZE \* 1024 \* 1024`

	if [ $TOTAL_SIZE -gt 3620000000 ]
	then
		print "MBR    : $MBR_SIZE"
		print "ROOTFS : $ROOTFS_SIZE"
		print "TOTAL  : $TOTAL_SIZE"
	else
		print "Insufficient disk capacity : $TOTAL_SIZE < 3620000000"
		run_end
	fi

	print "clean 0K~512K"
	# clean MBR
	dd if=/dev/zero of=$DEVNODE bs=1024 count=512
	
	print "clean 1M~64M"
	# clean PARTITION 0
	dd if=/dev/zero of=$DEVNODE bs=1M seek=1 count=63

	if [ $CLEANSET == "true" ]
	then
		dd if=/dev/zero of=$DEVNODE bs=1024 seek=768 count=1
	fi

	# bootloader: reboot or uboot
	# for Sandisk eMMC
	echo 0 > /sys/class/block/mmcblk0boot0/force_ro
	dd if=$UBOOT of=${DEVNODE}boot0 bs=1024 seek=1 || run_end
	sync
	echo 1 > /sys/class/block/mmcblk0boot0/force_ro
	
	# for Kingstom or Micro eMMC
	dd if=$UBOOT of=$DEVNODE bs=1024 seek=1 || run_end
	sync

	print "fdisk $DEVNODE"
	rm -f /.SD_PARTITION
	touch /.SD_PARTITION
	# - P1 ----------------------------------
	MBR_SIZE=`expr $MBR_SIZE / $UNITS_SIZE`
	ROOTFS_SIZE=`expr $MBR_SIZE + $ROOTFS_SIZE / $UNITS_SIZE`

	echo "		n" >> /.SD_PARTITION
	echo "		p" >> /.SD_PARTITION
	echo "		1" >> /.SD_PARTITION
	echo "		$MBR_SIZE" >> /.SD_PARTITION
	echo "		$ROOTFS_SIZE" >> /.SD_PARTITION
	echo "" >> /.SD_PARTITION

	# - Finish write-------------------------
	echo "		w" >> /.SD_PARTITION
	echo "" >> /.SD_PARTITION

	fdisk $DEVNODE < /.SD_PARTITION
	sync
	sleep 5

	print "format ${DEVNODE}p?"
	mke2fs -t ext4 -L root ${DEVNODE}p1
	sync

	# kernel image: uImage for recovery
	dd if=$RECOVERYKERNEL of=${DEVNODE} bs=1024 seek=1024 || run_end
	sync

	# kernel image dtb: uImage dtb for recovery
	dd if=$RECOVERYKERNELDTB of=${DEVNODE} bs=1024 seek=11776 || run_end
	sync

	# engramdisk image: engramdisk support uboot Hot key 'u'
	dd if=$RECOVERYRAMDISK of=${DEVNODE} bs=1024 seek=30720 || run_end
	sync

	# kernel image: uImage for normal
	dd if=$KERNEL of=${DEVNODE} bs=1024 seek=13312 || run_end
	sync

	# kernel image dtb: uImage dtb for normal
	dd if=$KERNELDTB of=${DEVNODE} bs=1024 seek=12800 || run_end
	sync

	if [ "$ROOTFS"x != "x" ]
	then
		print "  mount /dev/mmcblk0p1"
		mount -t ext4 /dev/mmcblk0p1 /mnt
		rm -rf /mnt/*
	
		print "  tar -xpvf ${ROOTFS}"
		tar -xpf ${ROOTFS} -C /mnt/
		sync

		if [ "$LIBS"x != "x" ] ; then
			print "  tar --numeric-owner -xpf ${LIBS}"
			rm -rf /mnt/lib/modules/*
			tar --numeric-owner -xpf ${LIBS} -C /mnt/
			modulename=`ls /mnt/lib/modules/`
			touch /mnt/lib/modules/${modulename}/flags.update_modules
			rm -f /mnt/lib/modules/${modulename}/build
			rm -f /mnt/lib/modules/${modulename}/source
		fi
	
		#tar -xpvf ${MODULE} -C /mnt/lib/modules/
		#sync
		echo $VERSION > /mnt/etc/version
		
		umount /mnt
		sync
	else
		run_end
	fi

	sleep 3
	sync
	sync
	sync
} > /dev/tty1

run
#mv /data/check_code /data/check_code-finish
sync
print "update system finished......"
sleep 3
print "Please Reboot System......"
sync
#reboot

