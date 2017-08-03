#! /bin/sh

MYPATH=`pwd`
DEVNODE="/dev/mmcblk0"

print() {
	echo $1 > /dev/ttymxc3
#	echo $1 > /dev/tty1
}

run_end() {
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

do_change_uid_gid() {
	chown $1:$2 $3
	uid=`stat -c '%u' $3`
	gid=`stat -c '%g' $3`
	if [ "$uid" != "$1" ] ; then
	    print "change uid $1 fail. get the uid is $uid"
	fi
	if [ "$gid" != "$2" ] ; then
	    print "change gid $2 fail. get the uid is $gid"
	fi
}

run() {
touch /tmp/restart
print "disable mcu watchdog"
/data/wdgdis

print "update system start........."
print "clean 32M........."
dd if=/dev/zero of=/dev/mmcblk0 bs=1M count=32 conv=fsync

echo 0 > /sys/block/mmcblk0boot0/force_ro
print "U-boot boot0........."
dd if=/data/flash/BSP_20170202.bin of=/dev/mmcblk0boot0 bs=1k seek=1 skip=1 count=1
sync
echo 1 > /sys/block/mmcblk0boot0/force_ro


#echo 0 > /sys/block/mmcblk0/force_ro
print "U-boot........."
dd if=/data/flash/BSP_20170202.bin of=/dev/mmcblk0 bs=1k seek=1 skip=1 count=1
sync

print "MBR........."
dd if=/data/flash/BSP_20170202.bin of=/dev/mmcblk0 bs=1024 count=512
sync
print "System......"
dd if=/data/flash/BSP_20170202.bin of=/dev/mmcblk0 bs=1M seek=1 skip=1
sync
#echo 1 > /sys/block/mmcblk0/force_ro
sync
} > /dev/ttymxc3

run

print "update system finished......(For $version)"


