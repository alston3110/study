#!/bin/sh

user_help() {
#	echo "dd script tool --- uboot, kernel, dtb, uboot prarmeter, filesystem, kernel modules to /dev/$1"
#	echo "parameter number $#, PID $$, file name $0"
	echo " command : $0 disk u	(uboot)"
	echo " command : $0 disk k	(kernel + dtb)"
	echo " command : $0 disk z	(/dev/zero)"
	echo " command : $0 disk m 	(cp lib,include to path)"
	echo " command : $0 disk file 	(tar file system)"
	echo " command : $0 disk all 	(ALL)"
	echo " Please check disk(sdb,sdc....)"
}

number=1
if [ -d linux-rtx ]; then
	file_path=$(readlink -f linux-rtx/out)
else
	echo " Can't find linux-rtx folder"
	exit 0
fi

sys_path=

if [ "$#" -eq "2" ]; then
	#Check Disk is correct!!
	if [ $(df -m | grep "/dev/$1$number" | wc -l) -gt "0" ]; then
		echo " Try to access dev/$1$number !!"
	else
		echo " Disk dev/$1$number don't exist!!"
		exit 0
	fi
	
	#Check Disk Size 
	if [ $(df -m | grep "/dev/$1$number" | awk '{print $2}') -gt "9000" ]; then
		echo " Disk dev/$1$number greater 9G, please re-comfirm size !!"
		exit 0
	fi

	if [ $(mount | grep "/dev/$1$number on /media" | wc -l) -eq "1" ]; then
		echo " Function option = $2"
		case $2 in
		u)
			echo " dd if=u-boot.imx of=/dev/$1 bs=512 seek=2"
			sudo dd if=$file_path/u-boot.imx of=/dev/$1 bs=512 seek=2
			sync
			break
			;;
		k)
			echo " dd kernel, dtb to of=/dev/$1"
			sudo dd if=$file_path/uImage of=/dev/$1 bs=512 seek=26624
			sync
			sudo dd if=$file_path/imx6q-rtx-pitx.dtb of=/dev/$1 bs=512 seek=25600
			sync
			break
			;;
		z)
			echo " dd if=/dev/zero of=/dev/$1 bs=512 seek=24576 count=10"
			sudo dd if=/dev/zero of=/dev/$1 bs=512 seek=24576 count=10
			sync
			break
			;;
		m)
			if [ $(ls $file_path | grep "lib.tar.bz2" | wc -l) -eq "1" ]; then
				sys_path=$(mount | grep "/dev/$1$number" | awk '{print $3}')
				echo " cp modules to $sys_path"
				sudo cp -avrf $file_path/lib $sys_path
				sync
			else
				echo " Can't find lib file!!"
			fi
			break
			;;
		file)
			echo " tar file"
			break
			;;
		all)
			echo " ALL"
			break
			;;	
		*)
			echo " Parameter $2 mistake !!"
			user_help
			break
			;;
		esac
	else
		echo " Disk mistake!!"
	fi
	
else
	echo " Parameter number error.... "
	user_help
	exit 0
fi


