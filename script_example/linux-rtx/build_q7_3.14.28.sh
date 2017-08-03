#! /bin/bash

set -e

#### Target OS #########################################################
TARGET_OS="Debian-8.5hf"
#TARGET_OS="Ubuntu-12.04el"
#TARGET_OS="Ubuntu-12.04hf"
#TARGET_OS="Ubuntu-14.04hf"
#TARGET_OS="Ubuntu-16.04hf"

#### Target Board ######################################################
#TARGET_BOARD="A6"
#TARGET_BOARD="A6Plus"
TARGET_BOARD="Q7"
#TARGET_BOARD="PITX"
#TARGET_BOARD="PITXUL"

#### Default Define ####################################################
KERNEL_DEFAULT_CONFIG=imx_v7_defconfig

BUILD_GPU_VIV_DRIVER_MODULE="no"
FIRMWARE_NAME="firmware-imx-3.14.28-1.0.0.bin"

TOP=`pwd`
OUT=${TOP}/out

#CROSS_COMPILE_PATH=/media/tom/ext1t/freescale/cross-compile
if [ -z $CROSS_COMPILE_PATH ];then
CROSS_COMPILE_PATH=/opt/cross
fi

########################################################################
export ARCH=arm

case "${TARGET_OS}" in
	"Debian-8.5hf")
		# Debian 8.4/8.5
		echo "Build Kernel for Debian 8.4/8.5 hf"
		export CROSS_COMPILE=${CROSS_COMPILE_PATH}/rtx-gcc-4.9.3-glibc-2.19-hf-64bits/arm-rtx-linux-gnueabihf/bin/arm-rtx-linux-gnueabihf-
		;;
	"Ubuntu-16.04hf")
		# Ubuntu 16.04
		echo "Build Kernel for Ubuntu 16.04 hf"
		export CROSS_COMPILE=${CROSS_COMPILE_PATH}/rtx-gcc-5.3.0-glibc-2.23-hf/arm-rtx-linux-gnueabihf/bin/arm-rtx-linux-gnueabihf-
		;;
	*)
		echo "Please set the target OS."
		exit 1
		;;
esac

case "${TARGET_BOARD}" in
	"A6" )
		TARGET_SOC=imx6q
		KERNEL_PROJECT_CONFIG=rtxconfig/rtx-a6-mx6q_defconfig
		# imx6s imx6dl imx6q
		KERNEL_LOADADDR=0x10008000
		KERNEL_DTB=imx6q-rtx-a6
		;;
	"A6Plus" )
		TARGET_SOC=imx6q
		KERNEL_PROJECT_CONFIG=rtxconfig/rtx-a6plus-mx6q_defconfig
		# imx6s imx6dl imx6q
		KERNEL_LOADADDR=0x10008000
		KERNEL_DTB=imx6q-rtx-a6plus
		;;
	"PITX" )
		TARGET_SOC=imx6q
		KERNEL_PROJECT_CONFIG=rtxconfig/rtx-pitx-mx6q_defconfig
		# imx6s imx6dl imx6q
		KERNEL_LOADADDR=0x10008000
		KERNEL_DTB=imx6q-rtx-pitx
		;;
	"Q7" )
		TARGET_SOC=imx6q
		KERNEL_PROJECT_CONFIG=rtxconfig/rtx-q7-mx6q_defconfig
		# imx6s imx6dl imx6q
		KERNEL_LOADADDR=0x10008000
		KERNEL_DTB=imx6q-rtx-q7
		;;
	*)
		echo "Please set the target board."
		exit 1
		;;
esac

KERNEL_VERSION=`make kernelversion`

function build_dir()
{
	for dir_item in out rtxconfig
	do
		if [ ! -d ${dir_item} ] ; then
			mkdir -p ${dir_item}
		fi
	done
}

function configure()
{
	if [ -z "${KERNEL_DEFAULT_CONFIG}" ] ; then
		echo "Please set the default configure file."
		exit 1
	fi
	
	if [ ! -z "${1}" ] ; then
		echo "Use the configure file (${1})."
		make distclean
		make ${1}
	else
		if [ ! -f .config ] ; then
			if [ -z "${KERNEL_PROJECT_CONFIG}" ] ; then
				echo "The project configure file (${KERNEL_PROJECT_CONFIG}) do not set."
				echo "Use the default configure file (${KERNEL_DEFAULT_CONFIG})."
				make ${KERNEL_DEFAULT_CONFIG}
			else
				if [ -f ${KERNEL_PROJECT_CONFIG} ] ; then
					echo "Use the project configure file (${KERNEL_PROJECT_CONFIG})."
					cp ${KERNEL_PROJECT_CONFIG} .config
				else
					echo "The project configure file (${KERNEL_PROJECT_CONFIG}) is not exist."
					echo "Use the default configure file (${KERNEL_DEFAULT_CONFIG})."
					make ${KERNEL_DEFAULT_CONFIG}
				fi
			fi
		fi
	fi
}

function build_kernel()
{
	if [ -z "${KERNEL_LOADADDR}" ] ; then
		make uImage -j8
	else
		make uImage -j8 LOADADDR=${KERNEL_LOADADDR}
	fi
	
	if [ -f arch/arm/boot/uImage ] ; then
		cp -f arch/arm/boot/uImage out/.
		if [ ! -z "${KERNEL_VERSION}" ] ; then
			cp -f arch/arm/boot/uImage out/uImage-${KERNEL_VERSION}
		fi
	fi
}

function build_dtb()
{
	if [ ! -z "${KERNEL_DTB}" ] ; then
		make ${KERNEL_DTB}.dtb
		cp arch/arm/boot/dts/${KERNEL_DTB}.dtb out/.
		if [ ! -z "${KERNEL_VERSION}" ] ; then
			cp arch/arm/boot/dts/${KERNEL_DTB}.dtb out/${KERNEL_DTB}-${KERNEL_VERSION}.dtb
		fi
	fi
}

function build_imx_firmware()
{
	case "${TARGET_SOC}" in
		"imx6q")
			if [ ! -d imx6-libs ] ; then
				break ;
			fi
			if [ ! -f imx6-libs/${FIRMWARE_NAME} ] ; then
				break ;
			fi
			if [ -d .tmp_build ] ; then
				rm -rf .tmp_build
			fi
			mkdir -p .tmp_build
			
			cp imx6-libs/${FIRMWARE_NAME} .tmp_build/.
			cd .tmp_build
			chmod +x ${FIRMWARE_NAME}
			./${FIRMWARE_NAME} --auto-accept --force
			cd ${TOP}
			mkdir -p out/lib/firmware/imx
			cp -rfv .tmp_build/firmware-imx-3.14.28-1.0.0/firmware/* out/lib/firmware/

			#find out/lib/firmware -type f -exec chmod 644 '{}' ';'
			#find out/lib/firmware -type f -exec chown root:root '{}' ';'

			# Remove files not going to be installed
			find out/lib/firmware/ -name '*.mk' -exec rm '{}' ';'
			rm -rf .tmp_build
		;;
	esac
}

function build_gpu_viv_module()
{
	if [ ${BUILD_GPU_VIV_DRIVER_MODULE} == "yes" ] ; then
		case "${TARGET_SOC}" in
			"imx6q")
				cd ${TOP}
				
				if [ ! -d imx6-libs ] ; then
					break ;
				fi
				if [ ! -f imx6-libs/kernel-module-imx-gpu-viv-5.0.11.p8.3.tar.gz ] ; then
					break ;
				fi
				if [ -d .tmp_build ] ; then
					rm -rf .tmp_build
				fi
				mkdir -p .tmp_build
				cd .tmp_build
				
				tar xzvf ${TOP}/imx6-libs/kernel-module-imx-gpu-viv-5.0.11.p8.3.tar.gz
				cd kernel-module-imx-gpu-viv-5.0.11.p8.3
				KERNEL_SRC=${TOP} make 
				KERNEL_SRC=${TOP} make DEPMOD=/bin/true INSTALL_MOD_PATH=${TOP}/out modules_install
				cd ${TOP}
				rm -rf .tmp_build
			;;
		esac
	fi
}

# Main function

# Build the necessary directions
build_dir

# 
case "${1}" in
	"all")
		rm -rf out
		mkdir -p out
		
		configure ${2}
		
		build_kernel
		build_dtb
		
		make modules -j8
		make DEPMOD=/bin/true INSTALL_MOD_PATH=out modules_install
		make INSTALL_FW_PATH=out/lib/firmware firmware_install
		make INSTALL_HDR_PATH=out headers_install
		
		build_gpu_viv_module
		build_imx_firmware
		
		cd out
		tar cvf lib.tar lib
		cd ..
		;;
	"config")
		configure ${2}
		;;
	"menuconfig")
		configure ${2}
		make menuconfig
		;;
	"diff")
		diff .config ${KERNEL_PROJECT_CONFIG}
		;;
	"save")
		if [ -f .config ] ; then
			cp .config ${KERNEL_PROJECT_CONFIG}
		fi
		;;
	"save-as")
		if [ -f .config ] ; then
			if [ -z "${2}" ] ; then
				echo "cmd save-as [filename]"
			else
				cp .config rtxconfig/${2}
			fi
		fi
		;;
	"load")
		if [ -z "${2}" ] ; then
			echo "cmd load [filename]"
		else
			cp rtxconfig/${2} .config
		fi
		;;
	"uImage")
		configure ${2}
		build_kernel
		build_dtb
		;;
	"dtb")
		configure ${2}
		build_dtb
		;;
	"modules")
		configure ${2}
		
		if [ ! -f arch/arm/boot/uImage ] ; then
			build_kernel
			build_dtb
		fi
		make modules -j8
		build_gpu_viv_module
		;;
	"install")
		if [ ! -d out ] ; then
			mkdir -p out
		fi
		
		configure ${2}
		
		if [ ! -f arch/arm/boot/uImage ] ; then
			build_kernel
			build_dtb
		fi
		if [ ! -f modules.builtin ] ; then
			make modules -j8
		fi
		make DEPMOD=/bin/true INSTALL_MOD_PATH=out modules_install
		make INSTALL_HDR_PATH=out headers_install
		cp -f arch/arm/boot/uImage out/.
		;;
	"clean")
		make clean
		;;
	"distclean")
		make distclean
		rm -rf out
		;;
	"rootfs")
		if [ -z "${2}" ] ; then
			echo "${0} ${1} [/dev/sd? or /dev/mmcblk?]"
		else
			sudo dd if=out/uImage of=${2} bs=512 seek=26624
			#echo "dd if=out/uImage of=${2} bs=512 seek=26624"
			if [ -d /media/rootfs/lib/modules ] ; then
				sudo rm -rf /media/rootfs/lib/modules/*
				sudo cp -avrf out/lib/modules/* /media/rootfs/lib/modules/.
				sudo chown -R root:root /media/rootfs/lib/modules/*
			fi
			if [ -d /media/rootfs/usr/src/linux/include ] ; then
				sudo rm -rf /media/rootfs/usr/src/linux/include/*
				sudo cp -avrf out/include/* /media/rootfs/usr/src/linux/include/.
				sudo chown -R root:root /media/rootfs/usr/src/linux/include/*
			fi
		fi
		;;
	*) 
		echo "${0} [all/config/menuconfig/saveconfig/uImage/modules/install/clean/disclean/rootfs]"
		exit 1
		;;
esac
