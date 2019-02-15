#!/bin/bash

OPEN_WRT_HOME=$2
COMMON_HOME="/opt/ejoin/rl"
VAD_STD_PREFIX="/opt/ejoin/vifi/vad"

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

TOP_SRC=$PWD

cd ..
VAD_HOME=$PWD
cd $TOP_SRC

VAD_PREFIX=${3:-$VAD_HOME}

chkdir $OPEN_WRT_HOME "openwrt not found"
#chkdir $VAD_PREFIX

chkdir $COMMON_HOME

COMMON_LIB=$COMMON_HOME/lib
chkdir $COMMON_LIB

echo "OPEN_WRT_HOME = $OPEN_WRT_HOME"
echo "VAD_PREFIX = $VAD_PREFIX"

config_env=$VAD_HOME/.config.env
echo -n "" > $config_env

ACOM_HW_TYPE="HWUUWIFI"
ACOM_PORT_NUM="2"
ACOM_BOARD_TYPE="RALINK"
ACOM_BIN_TYPE="vad"

BOARD_NAME="$(echo $1 | tr [A-Z\-] [a-z_])"

case "$1" in
  'RL-3G4G')
    ACOM_BOARD_SUBTYPE="RL00"
    ACOM_HW_SUBTYPE="HWST_NONE"
    ;;

  'RL-2G4G')
    ACOM_BOARD_SUBTYPE="RL01"
    ACOM_HW_SUBTYPE="HWST_2G4G"
    ;;
    
  'RL-2G4G-02')
    ACOM_BOARD_SUBTYPE="RL02"
    ACOM_HW_SUBTYPE="HWST_2G4G"
    ;;

  'RL-2G4G-03')
    ACOM_BOARD_SUBTYPE="RL03"
    ACOM_HW_SUBTYPE="HWST_2G4G"
    ;;  

  *)
    echo "device type \"$1\" is invalid!"
    exit
    ;;
esac

VAD_STD_PREFIX+=/$BOARD_NAME
echo "VAD_STD_PREFIX = $VAD_STD_PREFIX"

addConfig cc_home $OPEN_WRT_HOME
addConfig std_prefix $VAD_STD_PREFIX
addConfig board_name $BOARD_NAME
addConfig bin_type $ACOM_BIN_TYPE

addConfig pkg_name $ACOM_BIN_TYPE
addConfig fac_mode 0

mkdir_p $VAD_PREFIX/include $VAD_PREFIX/lib $VAD_PREFIX/bin

# copy the common liberaries
/bin/cp -af $COMMON_LIB/libeos* $COMMON_LIB/libeut* $VAD_PREFIX/lib/

# copy the common header files and libraries
[ "$VAD_PREFIX" != "$VAD_HOME" ] && {
  cp -afu $VAD_HOME/include $VAD_PREFIX/
  cp -afu $VAD_HOME/lib $VAD_PREFIX/
}

# copy all other libraries and binaries
[ "$VAD_PREFIX" != "$VAD_STD_PREFIX" ] && {
  for dir in $VAD_STD_PREFIX/lib $VAD_STD_PREFIX/bin $VAD_STD_PREFIX/include
  do
    # DON'T delete the option -u, it menas only cover the old existed files
    [ -d "$dir" ] && cp -afu $dir $VAD_PREFIX/
  done    
}

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

STAGING_DIR=$OPEN_WRT_HOME/staging_dir

# get the toolchain diretory
cd $STAGING_DIR/toolchain*
[ $? != 0 ] && echo "$STAGING_DIR/toolchain*: No such directory" && exit

TOOLSCHAIN=$PWD

# get the target directory
cd $STAGING_DIR/target-mipsel*
[ $? != 0 ] && echo "$STAGING_DIR/target-mipsel*: No such directory" && exit

TARGET=$PWD

cd $TOP_SRC

# define the os properties
EXTRA_DEFINE="-D_LITTLE_ENDIAN=1234 -D_BIG_ENDIAN=4321 -D_BYTE_ORDER=_LITTLE_ENDIAN "
EXTRA_DEFINE+="-DEOS_ARCH_LITTLE_ENDIAN=1234 -DEOS_ARCH_BIG_ENDIAN=4321 -DEOS_ARCH_ENDIAN=EOS_ARCH_LITTLE_ENDIAN "
EXTRA_DEFINE+="-D__POSIX_OS__ -DHAVE_CONFIG_H "

# define the values of board type
EXTRA_DEFINE+="-DKS8695=1 -DPPC405_4S=2 -DPPC405_8S16S=3 -DSAMSUNG4510B=4 -DPPC405_32S=5 -DLANTIQ=6 -DTI_AM335X=7 -DRALINK=8 -DMT673X=9 -DMDM9X07=10 "
EXTRA_DEFINE+="-DACOM_BOARD_TYPE=$ACOM_BOARD_TYPE "

# define the values of borad subtype
EXTRA_DEFINE+="-DRL00=0 -DRL01=1 -DRL02=2 -DRL03=3 -DMT_U3=10 -DMT_U4=11 -DMDM_01=20 "
EXTRA_DEFINE+="-DACOM_BOARD_SUBTYPE=$ACOM_BOARD_SUBTYPE "

# define the values of hardware type
EXTRA_DEFINE+="-DHWKS86954S=1 -DHWKS86958S=2 -DHWKS8695INFINEON2S=3 -DHWPPC4054S=4 -DHWPPC40516S=5 -DHWSAMSUNG4510B2S=6 -DHWPPC40532S=7 -DHWGOIP=8 -DHWVOIP=9 -DHWSIMPOOL=10 -DHWUUWIFI=11 "
EXTRA_DEFINE+="-DACOM_HW_TYPE=$ACOM_HW_TYPE -DACOM_PORT_NUM=$ACOM_PORT_NUM "

# define the values of sub-hw type
EXTRA_DEFINE+="-DHWST_NONE=0 -DHWST_3G4G=0 -DHWST_2G4G=1 "
EXTRA_DEFINE+="-DACOM_HW_SUBTYPE=$ACOM_HW_SUBTYPE "

EXTRA_DEFINE+="-g -O0 "
EXTRA_DEFINE+="-Wno-unused-but-set-variable "

CC=gcc \
LIBC=$TOOLSCHAIN/lib \
LDFLAGS="-L$TOOLSCHAIN/lib -L$TOOLSCHAIN/usr/lib -L$TARGET/usr/lib" \
./configure --enable-shared=yes --prefix=$VAD_PREFIX \
 --disable-werror --disable-gdbtk --disable-tui \
 --without-x --without-included-regex --without-included-gettext \
 --with-board-type=$ACOM_BOARD_TYPE --with-hw-type=$ACOM_HW_TYPE --with-oem-name=$ACOM_OEM_NAME
