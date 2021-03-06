#! /bin/sh

CLEANSET="false"

SYSPATH="/data"
DATAPATH="/data/flash"
DEVNODE_NAME="mmcblk0"
DEVNODE="/dev/${DEVNODE_NAME}"
DEBUG_PORT="/dev/ttymxc0"
VERSION="rtx-q7-android-4.4.2-001"

#### Partition information ####

PARTITIONS="0:s:64 1:p:512 2:p:512 3:e:128 4:p:- 5:l:32 6:l:-"
let LIMIT_SIZE=3620*1000*1000
P0_PARTITION="2:-:${DATAPATH}/u-boot.imx:0 2048:-:${SYSPATH}/uImage-recovery:0 23552:-:${SYSPATH}/recovery.dtb:0 26624:-:${DATAPATH}/uImage:0 49152:-:${DATAPATH}/ramdisk.img.uramdisk:0 53248:-:${DATAPATH}/ramdisk-recovery.img.uramdisk:0 61440:-:${SYSPATH}/uramdisk-recovery.img":0
PARTITIONS_FS="1:ext4:SYSTEM 2:ext4:CACHE 4:ext4:DATA 5:ext4:DEVICE 6:ext4:MISC"

P1_FS_SCRIPT="1:tar:${DATAPATH}/system.tar.gz"
FS_SCRIPTS="P1_FS_SCRIPT"

# =====================================================================================

# -------------------------------------------------------------------------------------
print()
{
    echo $1 > /dev/tty1
    echo $1 > ${DEBUG_PORT}
}

p0_cls()
{
    print "clean 0K~512K"
    # clean MBR
    dd if=/dev/zero of=${DEVNODE} bs=1024 count=512
	
    print "clean 1M~64M"
    # clean PARTITION 0
    dd if=/dev/zero of=${DEVNODE} bs=1M seek=1 count=63

    if [ $CLEANSET == "true" ]
    then
        dd if=/dev/zero of=${DEVNODE} bs=1024 seek=768 count=1
    fi

}

mk_fdisk() {
    TOTAL_SIZE=`fdisk -l ${DEVNODE} | grep 'Disk /dev/' | awk '{print $5}'`
    UNITS_SIZE=`fdisk -l ${DEVNODE} | grep 'Units = cylinders' | awk '{print $9}'`

    if [ $TOTAL_SIZE -lt ${LIMIT_SIZE} ]
    then
        print "Target ${DEVNODE} size too small"
        exit 1
    fi
    print "Total Size:${TOTAL_SIZE} Unit Size:${UNITS_SIZE}"

    rm -f /.SD_PARTITION
    touch /.SD_PARTITION

    COUNT=0
    S_SECTOR=0
    E_SECTOR=0
    EXT_S_SECTOR=0
    EXT_E_SECTOR=0
    EXT_T_SECTOR=0
    let T_SECTOR=LIMIT_SIZE/UNITS_SIZE

    for P_INFO in ${PARTITIONS}
    do
        P_NUM=${P_INFO:0:1}
        P_TYPE=${P_INFO:2:1}
        P_SIZE=${P_INFO:4}
        if [ ${P_NUM} -le 4 ]
        then
            if [ ${COUNT} -eq ${P_NUM} ]
            then
                if [ ${P_NUM} -eq 0 ]
                then
                    let S_SECTOR=P_SIZE*1024*1024/UNITS_SIZE
                    print "MBR : ${P_SIZE} MB -> Sectors : ${S_SECTOR}"
                else
                    if [ ${P_SIZE} = "-" ]
                    then
                        print "P${P_NUM} : ${P_SIZE} MB -> Start Sector : ${S_SECTOR} , End Sector : ${T_SECTOR}"
            	    
	                echo "		n" >> /.SD_PARTITION
                        if [ ${P_TYPE} = "p" ]
                        then
                            echo "		p" >> /.SD_PARTITION
                        else
                            echo "		e" >> /.SD_PARTITION
                            let EXT_S_SECTOR=S_SECTOR
                            let EXT_T_SECTOR=T_SECTOR
                        fi
                        echo "		${S_SECTOR}" >> /.SD_PARTITION
                        echo "		${T_SECTOR}" >> /.SD_PARTITION
                        echo "" >> /.SD_PARTITION
                    else
                        let E_SECTOR=S_SECTOR+P_SIZE*1024*1024/UNITS_SIZE-1
                        print "P${P_NUM} : ${P_SIZE} MB -> Start Sector : ${S_SECTOR} , End Sector : ${E_SECTOR}"
            	    
                        echo "		n" >> /.SD_PARTITION
                        if [ ${P_TYPE} = "p" ]
                        then
                            echo "		p" >> /.SD_PARTITION
                        else
                            echo "		e" >> /.SD_PARTITION
                            let EXT_S_SECTOR=S_SECTOR
                            let EXT_T_SECTOR=E_SECTOR
                        fi
                        echo "		${P_NUM}" >> /.SD_PARTITION
                        echo "		${S_SECTOR}" >> /.SD_PARTITION
                        echo "		${E_SECTOR}" >> /.SD_PARTITION
                        echo "" >> /.SD_PARTITION
                        let S_SECTOR=E_SECTOR+1
                    fi
                fi
            else
                print "[Error] Partitions information was incorrect"
                exit 1
            fi
        else
            if [ ${EXT_SECTOR} -eq 0 ]
            then
                print "[Error] No extern partition"
            else
                if [ ${P_SIZE} = "-" ]
                then
                    print "P${P_NUM} : ${P_SIZE} MB -> Start Sector : ${EXT_S_SECTOR} , End Sector : ${EXT_T_SECTOR}"
                    echo "		n" >> /.SD_PARTITION
                    echo "		${EXT_S_SECTOR}" >> /.SD_PARTITION
                    echo "		${EXT_T_SECTOR}" >> /.SD_PARTITION
                    echo "" >> /.SD_PARTITION
                else
                    let EXT_E_SECTOR=EXT_S_SECTOR+P_SIZE*1024*1024/UNITS_SIZE-1
                    print "P${P_NUM} : ${P_SIZE} MB -> Start Sector : ${EXT_S_SECTOR} , End Sector : ${EXT_E_SECTOR}"
                    echo "		n" >> /.SD_PARTITION
                    echo "		${EXT_S_SECTOR}" >> /.SD_PARTITION
                    echo "		${EXT_E_SECTOR}" >> /.SD_PARTITION
                    echo "" >> /.SD_PARTITION
            	    let EXT_S_SECTOR=EXT_E_SECTOR+1
                fi
            fi
        fi
        let COUNT=COUNT+1
    done

    # - Finish write-------------------------
    echo "		w" >> /.SD_PARTITION
    echo "" >> /.SD_PARTITION
    
    # - run fdisk ---------------------------
    print "run fdisk"
	fdisk ${DEVNODE} < /.SD_PARTITION
	sync
	sleep 1
}

partitions_formate()
{
	print "Format partition"
	
    for PARTITION_FS in ${PARTITIONS_FS}
    do
        P_NUM=`echo ${PARTITION_FS} | awk -F':' '{print $1}'`
        P_TYPE=`echo ${PARTITION_FS} | awk -F':' '{print $2}'`
        P_LABEL=`echo ${PARTITION_FS} | awk -F':' '{print $3}'`
        P_ARGS=`echo ${PARTITION_FS} | awk -F':' '{print $4}'`
        
        print "mkfs.${P_TYPE} ${DEVNODE}p${P_NUM} -L/-n ${P_LABEL} ${P_ARGS}"
        
        if [ -b ${DEVNODE}p${P_NUM} ]
        then
            case ${P_TYPE} in
                "ext2")
                    if [ "${P_ARGS}" == "" ]
                    then
                        mkfs.ext2 ${DEVNODE}p${P_NUM} -L ${P_LABEL}
                    else
                        mkfs.ext2 ${DEVNODE}p${P_NUM} -L ${P_LABEL} ${P_ARGS}
                    fi
                    ;;
                "ext3")
                    if [ "${P_ARGS}" == "" ]
                    then
                        mkfs.ext3 ${DEVNODE}p${P_NUM} -L ${P_LABEL}
                    else
                        mkfs.ext3 ${DEVNODE}p${P_NUM} -L ${P_LABEL} ${P_ARGS}
                    fi
                    ;;
                "ext4")
                    if [ "${P_ARGS}" == "" ]
                    then
                        mkfs.ext4 ${DEVNODE}p${P_NUM} -L ${P_LABEL}
                    else
                        mkfs.ext4 ${DEVNODE}p${P_NUM} -L ${P_LABEL} ${P_ARGS}
                    fi
                    ;;
                "vfat")
                    if [ "${P_ARGS}" == "" ]
                    then
                        mkfs.vfat ${DEVNODE}p${P_NUM} -n ${P_LABEL}
                    else
                        mkfs.vfat ${DEVNODE}p${P_NUM} -n ${P_LABEL} ${P_ARGS}
                    fi
                    ;;
                *)
                    print "Unknow cmd"
                    exit 1
                    ;;
            esac
        else
            print "${DEVNODE}p${P_NUM} was not exist."
            exit 1
        fi
    done
    
    sync
    sleep 1
}

p0_write()
{    
    for P0_INFO in ${P0_PARTITION}
    do
        echo ${P0_INFO}
        P0_S_SECTOR=`echo ${P0_INFO} | awk -F':' '{print $1}'`
        P0_E_SECTOR=`echo ${P0_INFO} | awk -F':' '{print $2}'`
        P0_FILE=`echo ${P0_INFO} | awk -F':' '{print $3}'`
        P0_FILE_SKIP=`echo ${P0_INFO} | awk -F':' '{print $4}'`
        
        print "dd if=${P0_FILE} of=${DEVNODE} bs=512 seek=${P0_S_SECTOR} skip=${P0_FILE_SKIP}"
        
        if [ ! -f ${P0_FILE} ]
        then
            print "Can't find the file (${P0_FILE})"
            exit 1
        fi
        if [ ! "${P0_E_SECTOR}" == "-" ]
        then
            FILE_SIZE=`stat -c '%s' ${P0_FILE}`
            FILE_SECTOR=0
            let FILE_SECTOR=FILE_SIZE+511
            let FILE_SECTOR=FILE_SECTOR/512
            let P0_DE_SECTOR=P0_S_SECTOR+FILE_SECTOR
            if [ ${P0_E_SECTOR} -lt ${P0_DE_SECTOR} ]
            then
                print "dd file to large (${P0_E_SECTOR} < ${P0_DE_SECTOR})"
                exit 1
            fi
        fi
        
        if [ ${P0_S_SECTOR} == "2" ]
        then
            echo "0" > /sys/block/${DEVNODE_NAME}boot0/force_ro
            if [ ${P0_FILE_SKIP} == "0" ]
            then
                dd if=${P0_FILE} of=${DEVNODE}boot0 bs=512 seek=${P0_S_SECTOR}
            else
                dd if=${P0_FILE} of=${DEVNODE}boot0 bs=512 seek=${P0_S_SECTOR} skip=${P0_FILE_SKIP}
            fi
        fi
        
        if [ ${P0_FILE_SKIP} == "0" ]
        then
            dd if=${P0_FILE} of=${DEVNODE} bs=512 seek=${P0_S_SECTOR}
        else
            dd if=${P0_FILE} of=${DEVNODE} bs=512 seek=${P0_S_SECTOR} skip=${P0_FILE_SKIP}
        fi
        
        if [ ${P0_S_SECTOR} == "2" ]
        then
            echo "1" > /sys/block/${DEVNODE_NAME}boot0/force_ro
        fi
    done
    sync
    sleep 1
}

fs_write()
{
    for FS_SCRIPT in ${FS_SCRIPTS}
    do
    	echo ${FS_SCRIPT}
    	eval RUN_SCRIPT=\$${FS_SCRIPT}
    	echo ${RUN_SCRIPT}
        P_NUM=`echo ${RUN_SCRIPT} | awk -F':' '{print $1}'`
        R_CMD=`echo ${RUN_SCRIPT} | awk -F':' '{print $2}'`
        S_FILE=`echo ${RUN_SCRIPT} | awk -F':' '{print $3}'`
        R_ARGS=`echo ${RUN_SCRIPT} | awk -F':' '{print $4}'`
        
        if [ ! -f ${S_FILE} ]
        then
            print "Can't find the file (${S_FILE})"
            exit 1
        fi
        if [ -b ${DEVNODE}p${P_NUM} ]
        then
            case ${R_CMD} in
                "tar")
                    print "tar ${R_ARGS} --numeric-owner -pxvf ${S_FILE} -C /mnt"
                    mount ${DEVNODE}p${P_NUM} /mnt
                    if [ "${R_ARGS}" == "" ]
                    then
                        tar --numeric-owner -pxvf ${S_FILE} -C /mnt
                    else
                        tar ${R_ARGS} --numeric-owner -pxvf ${S_FILE} -C /mnt
                    fi
                    sync
                    umount /mnt
                    ;;
                "dd" )
                    print "dd if=${S_FILE} of=${DEVNODE}p${P_NUM} ${R_ARGS}"
                    if [ "${R_ARGS}" == "" ]
                    then
                        dd if=${S_FILE} of=${DEVNODE}p${P_NUM}
                    else
                        dd if=${S_FILE} of=${DEVNODE}p${P_NUM} ${R_ARGS}
                    fi
                    sync
                    ;;
                *)
                    print "Unknow cmd"
                    exit 1
                    ;;
            esac
        else
            print "${DEVNODE}p${P_NUM} was not exist."
            exit 1
        fi
    done
    sync
    sleep 1
}

p0_cls

mk_fdisk

partitions_formate

p0_write

fs_write

sync
print "update system finished......"
sleep 3
print "Please Reboot System......"
sync

