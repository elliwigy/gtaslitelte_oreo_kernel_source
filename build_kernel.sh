#!/bin/bash

OUT_DIR=out

COMMON_ARGS="-C $(pwd) O=$(pwd)/${OUT_DIR} ARCH=arm CROSS_COMPILE=$(pwd)/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.9/bin/arm-linux-androideabi- KCFLAGS=-mno-android"

export PATH=$(pwd)/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.9/bin:$PATH

export USE_CCACHE=1
export CCACHE_DIR=/media/elliwigy/sampwnd/.cache
ccache -M 50G

export ARCH=arm

[ -d ${OUT_DIR} ] && rm -rf ${OUT_DIR}
mkdir ${OUT_DIR}

make ${COMMON_ARGS} gtaslitelte_usa_spr_defconfig
make -j64 ${COMMON_ARGS}

cp ${OUT_DIR}/arch/arm/boot/zImage $(pwd)/arch/arm/boot/zImage
