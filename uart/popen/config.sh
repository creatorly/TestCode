#!/bin/bash

chkdir() {
  prompt=${2:-"no such directory"}
  [ ! -d "$1" ] && echo "$1: $prompt" && exit
}

mkdir_p() {
  for dirname in $*
  do
    [ ! -d "$dirname" ] && {
      echo "Create directory $dirname ..."
      mkdir -p $dirname
    }
  done
}

addConfig() {
  echo "$1=$2" >> $config_env
}

VAD_HOME=$PWD

VAD_PREFIX=${3:-$VAD_HOME}

echo "VAD_PREFIX = $VAD_PREFIX"

config_env=$VAD_HOME/.config.env
echo -n "" > $config_env

ACOM_HW_TYPE="HWUUWIFI"
ACOM_PORT_NUM="2"
ACOM_BOARD_TYPE="RALINK"
ACOM_BIN_TYPE="vad"

BOARD_NAME="$(echo $1 | tr [A-Z\-] [a-z_])"

addConfig board_name $BOARD_NAME
addConfig bin_type $ACOM_BIN_TYPE

addConfig pkg_name $ACOM_BIN_TYPE
addConfig fac_mode 0

MAKEFILES=`grep '/[Mm]akefile *$' configure.ac`

# bakup the configure.ac
/bin/cp -f configure.ac configure.ac.bak

# remove the Makefiles whose path not exist
for mkf in $MAKEFILES
do
  if [ ! -f "$mkf.am" -a ! -f "$mkf.in" ] ;then
    # delete the Makefile output file
    sed -ri "s|$mkf||g" configure.ac
    # create the directory for autoreconf
    mkdir -p ${mkf%/*}
  fi
done

autoreconf -if
ac_res=$?

# restore the configure.ac
mv -f configure.ac.bak configure.ac

[ "$ac_res" != "0" ] && exit


# generate the mips cache file "mips-linux.cache"
echo "ac_cv_func_malloc_0_nonnull=yes" > mips-linux.cache

# define the os properties
EXTRA_DEFINE="-D_LITTLE_ENDIAN=1234 -D_BIG_ENDIAN=4321 -D_BYTE_ORDER=_LITTLE_ENDIAN "
EXTRA_DEFINE+="-DEOS_ARCH_LITTLE_ENDIAN=1234 -DEOS_ARCH_BIG_ENDIAN=4321 -DEOS_ARCH_ENDIAN=EOS_ARCH_LITTLE_ENDIAN "
EXTRA_DEFINE+="-D__POSIX_OS__ -DHAVE_CONFIG_H "

# define the values of board type
EXTRA_DEFINE+="-DKS8695=1 -DPPC405_4S=2 -DPPC405_8S16S=3 -DSAMSUNG4510B=4 -DPPC405_32S=5 -DLANTIQ=6 -DTI_AM335X=7 -DRALINK=8 -DMT673X=9 -DMDM9X07=10 "
EXTRA_DEFINE+="-DACOM_BOARD_TYPE=$ACOM_BOARD_TYPE "

# define the values of borad subtype
EXTRA_DEFINE+="-DRL00=0 -DRL01=1 -DRL02=2 -DRL03=3 -DMT_U3=10 -DMT_U4=11 -DMDM_01=20 "

# define the values of hardware type
EXTRA_DEFINE+="-DHWKS86954S=1 -DHWKS86958S=2 -DHWKS8695INFINEON2S=3 -DHWPPC4054S=4 -DHWPPC40516S=5 -DHWSAMSUNG4510B2S=6 -DHWPPC40532S=7 -DHWGOIP=8 -DHWVOIP=9 -DHWSIMPOOL=10 -DHWUUWIFI=11 "
EXTRA_DEFINE+="-DACOM_HW_TYPE=$ACOM_HW_TYPE -DACOM_PORT_NUM=$ACOM_PORT_NUM "

# define the values of sub-hw type
EXTRA_DEFINE+="-DHWST_NONE=0 -DHWST_3G4G=0 -DHWST_2G4G=1 "

EXTRA_DEFINE+="-g -O0 "
EXTRA_DEFINE+="-Wno-unused-but-set-variable "


CC=gcc \
./configure --enable-shared=yes --prefix=$VAD_PREFIX \
 --disable-werror --disable-gdbtk --disable-tui \
 --without-x --without-included-regex --without-included-gettext \
 --with-board-type=$ACOM_BOARD_TYPE --with-hw-type=$ACOM_HW_TYPE --with-oem-name=$ACOM_OEM_NAME
