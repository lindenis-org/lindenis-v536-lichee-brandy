#!/bin/bash
set -e

PLATFORM="sun8iw3p1"
MODE=""
OUTPUT=""

localpath=$(cd $(dirname $0) && pwd)
buildconfig=$localpath/../../../.buildconfig
argnum=$#

BRANDY_SPL_DIR=$localpath/../bootloader

show_help()
{
	printf "\nbuild.sh - Top level build scritps\n"
	echo "Valid Options:"
	echo "  -h  Show help message"
	echo "  -p <platform> platform, e.g. sun5i, sun6i, sun8iw1p1, sun8iw3p1, sun9iw1p1"
	echo "  -m <mode> mode, e.g. ota_test"
	echo "  -o <output> output, e.g. uboot, uboot_nor, boot0"
	echo "  -t install gcc tools chain"
	printf "\n\n"
}

prepare_toolchain()
{
        local ARCH="arm";
        local GCC="";
        local GCC_PREFIX="";
        local toolchain_archive_aarch64="./toolchain/gcc-linaro-aarch64.tar.xz";
        local toolchain_archive_arm="./toolchain/gcc-linaro-arm.tar.xz";
        local tooldir_aarch64="./toolchain/gcc-aarch64";
        local tooldir_arm="./toolchain/gcc-arm";

        echo "Prepare toolchain ..."

        if [ ! -d "${tooldir_aarch64}" ]; then
                mkdir -p ${tooldir_aarch64} || exit 1
                tar --strip-components=1 -xf ${toolchain_archive_aarch64} -C ${tooldir_aarch64} || exit 1
        fi

        if [ ! -d "${tooldir_arm}" ]; then
                mkdir -p ${tooldir_arm} || exit 1
                tar --strip-components=1 -xf ${toolchain_archive_arm} -C ${tooldir_arm} || exit 1
        fi
}

select_uboot()
{
	if [ "x${PLATFORM}" = "xsun50iw1p1" ] || \
	[ "x${PLATFORM}" = "xsun50iw2p1" ] || \
	[ "x${PLATFORM}" = "xsun50iw6p1" ] || \
	[ "x${PLATFORM}" = "xsun50iw3p1" ] || \
	[ "x${PLATFORM}" = "xsun3iw1p1"  ] || \
	[ "x${PLATFORM}" = "xsun8iw12p1" ] || \
	[ "x${PLATFORM}" = "xsun8iw10p1" ] || \
	[ "x${PLATFORM}" = "xsun8iw11p1" ] || \
	[ "x${PLATFORM}" = "xsun8iw12p1" ] || \
	[ "x${PLATFORM}" = "xsun8iw6p1" ] || \
	[ "x${PLATFORM}" = "xsun8iw15p1" ] || \
	[ "x${PLATFORM}" = "xsun8iw15p1_axp2231" ] || \
	[ "x${PLATFORM}" = "xsun8iw16p1" ] || \
	[ "x${PLATFORM}" = "xsun8iw17p1" ];then
		if [ x"${TARGET_PLATFORM}" = x"banjo" ]; then
			echo "u-boot-2011.09"
		else
			echo "u-boot-2014.07"
		fi
	else
		echo "u-boot-2011.09"
	fi
}

get_platform()
{
	if [ $argnum -eq 0 ] && [ -f ${buildconfig} ]; then
		local LICHEE_CHIP=`cat ${buildconfig} | grep -w "LICHEE_CHIP" | awk -F= '{printf $2}'`
		local LICHEE_BUSSINESS=`cat ${buildconfig} | grep -w "LICHEE_CHIP" | awk -F= '{printf $2}'`
		if [ -n "$LICHEE_CHIP" ]; then
			PLATFORM=$LICHEE_CHIP
			if [ -n "$LICHEE_BUSSINESS" ]; then
				local boardcfg=$localpath/$(select_uboot)/boards.cfg
				local parten=1
				[ $(select_uboot) == "u-boot-2014.07" ] && parten=7
				[ -n "$(sed -e '/^#/d' $boardcfg | awk '{print $'"$parten"'}' | \
					grep ${PLATFORM}_${LICHEE_BUSSINESS})" ] && \
				PLATFORM=${PLATFORM}_${LICHEE_BUSSINESS}
			fi
		fi
		echo "Get PLATFORM from lichee buildconfig: $PLATFORM"

	elif [ $argnum -eq 0 ] && [ "x${TARGET_BUILD_VARIANT}" = "xtina" ] ; then
		local LICHEE_CHIP=$TARGET_CHIP
		local LICHEE_BUSSINESS=$TARGET_CHIP
		if [ -n "$LICHEE_CHIP" ]; then
			PLATFORM=$LICHEE_CHIP
			if [ -n "$LICHEE_BUSSINESS" ]; then
				local boardcfg=$localpath/$(select_uboot)/boards.cfg
				local parten=1
				[ $(select_uboot) == "u-boot-2014.07" ] && parten=7
				[ -n "$(sed -e '/^#/d' $boardcfg | awk '{print $'"$parten"'}' | \
					grep ${PLATFORM}_${LICHEE_BUSSINESS})" ] && \
				PLATFORM=${PLATFORM}_${LICHEE_BUSSINESS}
			fi
		fi

	fi
}

build_uboot()
{
	get_platform

	cd $(select_uboot)

	make distclean
	if [ "x$MODE" = "xota_test" ] ; then
		export "SUNXI_MODE=ota_test"
	fi
	make ${PLATFORM}_config
	if [ "x$OUTPUT" = "xuboot" -o "x$OUTPUT" = "x" ] ; then
		make -j16
	fi
	#spl
	if [ -d ${BRANDY_SPL_DIR} ] ; then
		if [ "x$OUTPUT" = "xboot0" -o "x$OUTPUT" = "x" ] ; then
			make spl
		fi
	fi

	if [ "x$OUTPUT" = "xuboot" -o x$OUTPUT = "xuboot_nor" -o  x$OUTPUT = "x" ] ; then
		if [ ${PLATFORM} = "sun8iw16p1" ] || \
		[ ${PLATFORM} = "sun8iw12p1"    ] || \
		[ ${PLATFORM} = "sun3iw1p1"     ] || \
		[ ${PLATFORM} = "sun8iw8p1" ]; then
			make distclean
			make ${PLATFORM}_nor_config
			make -j16
		fi
	fi

	cd - 1>/dev/null
}

while getopts p:m:o:t OPTION
do
	case $OPTION in
	p)
		PLATFORM=$OPTARG
		;;
	m)
		MODE=$OPTARG
		;;
	o)
		OUTPUT=$OPTARG
		;;
	t)
		prepare_toolchain
		exit
		;;
	*) show_help
		exit
		;;
esac
done


build_uboot




