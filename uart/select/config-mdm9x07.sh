#!/bin/bash

echo -e "\nusage: $0 [board_type [mdm9x07_home [vad_prefix [extra_define [config_options]]]]]"

################################################################################
# define the local functions
################################################################################

chkdir() {
  prompt=${2:-"no such directory"}
  [ ! -d "$1" ] && echo "$1: $prompt" && exit 1
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


############################################################################
# set the environment variables
############################################################################

VAD_HOME=$PWD

BOARD_TYPE=${1:-MDM9X07-01}
BOARD_NAME="$(echo $BOARD_TYPE | tr [A-Z\-] [a-z_])"

COMMON_HOME="/opt/ejoin/mdm9x07"
MDM9x07_HOME=${2:-"/opt/ejoin/tools/mdm9x07"}
VAD_PREFIX=${3:-$VAD_HOME}
VAD_STD_PREFIX="/opt/ejoin/vifi/vad/$BOARD_NAME"

EXTRA_DEFINE="$4 "

for((i=5;i<=$#;i++))
do
  eval arg="\$$i"
  config_options+=" $arg"
done

echo -e "\nBOARD_NAME = $BOARD_NAME"
echo "MDM9x07_HOME = $MDM9x07_HOME"
echo "VAD_PREFIX = $VAD_PREFIX"
echo -e "VAD_STD_PREFIX = $VAD_STD_PREFIX\n"


###########################################################################
# check theh mdm9x07 and common home directory
#
chkdir $MDM9x07_HOME "$MDM9x07_HOME: no mdm9x07 home diretory"

# SYSROOT=$MDM9x07_HOME/mdm9607
SYSROOT=$MDM9x07_HOME/mdm9607-perf
chkdir $SYSROOT "$SYSROOT: no sysroot direcotry"

TOOLSCHAIN_BIN=$MDM9x07_HOME/x86_64-linux/usr/bin/arm-oe-linux-gnueabi
chkdir $TOOLSCHAIN_BIN "$TOOLSCHAIN_BIN: no toolschain binary diretory"

chkdir $COMMON_HOME

COMMON_LIB=$COMMON_HOME/lib
chkdir $COMMON_LIB


###########################################################################
# set the vad project environment variables
##

config_env=$VAD_HOME/.config.env
echo -n "" > $config_env

ACOM_HW_TYPE="HWUUWIFI"
ACOM_PORT_NUM="3"

ACOM_BOARD_TYPE="MDM9X07"

case "$BOARD_TYPE" in
  'MDM9X07-01')
    ACOM_BOARD_SUBTYPE="MDM_01"
    ACOM_HW_SUBTYPE="HWST_2G4G"
    ACOM_BIN_TYPE="vad_mdm01"
    ;;

  'MDM9X07-02')
    ACOM_BOARD_SUBTYPE="MDM_02"
    ACOM_HW_SUBTYPE="HWST_2G4G"
    ACOM_BIN_TYPE="vad_mdm02"
    ;;

  'MDM9X07-03')
    ACOM_BOARD_SUBTYPE="MDM_03"
    ACOM_HW_SUBTYPE="HWST_2G4G"
    ACOM_BIN_TYPE="vad_mdm03"
    ;;

  'MDM9X07-04')
    ACOM_BOARD_SUBTYPE="MDM_04"
    ACOM_HW_SUBTYPE="HWST_2G4G"
    ACOM_BIN_TYPE="vad_mdm04"
    ;;

  'MDM9X07-05')
    ACOM_BOARD_SUBTYPE="MDM_05"
    ACOM_HW_SUBTYPE="HWST_2G4G"
    ACOM_BIN_TYPE="vad_mdm05"
    ;;

  *)
    echo "device type \"$1\" is invalid!"
    exit 1
    ;;
esac

addConfig cc_home $MDM9x07_HOME
addConfig std_prefix $VAD_STD_PREFIX
addConfig board_name $BOARD_NAME
addConfig bin_type $ACOM_BIN_TYPE

echo "$EXTRA_DEFINE" | grep '\-DVAD_FAC_CHK_SIGNAL=1' 1>/dev/null 2>&1
if [ $? = 0 ] ; then
  addConfig pkg_name $ACOM_BIN_TYPE-fac
  addConfig fac_mode 1
else
  addConfig pkg_name $ACOM_BIN_TYPE
  addConfig fac_mode 0
fi

#############################################################################
# generate the file of configure automatically
#
MAKEFILES=`grep '/[Mm]akefile *$' configure.ac`

# bakup the configure.ac
/bin/cp -f configure.ac configure.ac.bak

# remove the Makefiles whose path not exist
for mkf in $MAKEFILES
do
  if [ ! -f "$mkf.am" -a ! -f "$mkf.in" ] ;then
    # delete the Makefile output file
    sed -ri "s|^\s*$mkf||g" configure.ac
    # create the directory for autoreconf
    mkdir -p ${mkf%/*}
  fi
done

# generate the arm cache file "arm-linux.cache"
# ifnone the autotools will think the function malloc not exists
echo "ac_cv_func_malloc_0_nonnull=yes" > arm-linux.cache

autoreconf -if
ac_res=$?

# restore the configure.ac
mv -f configure.ac.bak configure.ac

[ "$ac_res" != "0" ] && exit


###############################################################################################################
# define the os properties
#
EXTRA_DEFINE+="-D_LITTLE_ENDIAN=1234 -D_BIG_ENDIAN=4321 -D_BYTE_ORDER=_LITTLE_ENDIAN "
EXTRA_DEFINE+="-DEOS_ARCH_LITTLE_ENDIAN=1234 -DEOS_ARCH_BIG_ENDIAN=4321 -DEOS_ARCH_ENDIAN=EOS_ARCH_LITTLE_ENDIAN "
EXTRA_DEFINE+="-D__POSIX_OS__ -DHAVE_CONFIG_H "
EXTRA_DEFINE+="-g -O0 "
# EXTRA_DEFINE+="-Os "
EXTRA_DEFINE+="-Wall -Wno-unused-but-set-variable -Wformat-security "

# define the values of board type
EXTRA_DEFINE+="-DKS8695=1 -DPPC405_4S=2 -DPPC405_8S16S=3 -DSAMSUNG4510B=4 -DPPC405_32S=5 -DLANTIQ=6 -DTI_AM335X=7 -DRALINK=8 -DMT673X=9 -DMDM9X07=10 -DMSM89XX=11 -DSC98XX=12 "
EXTRA_DEFINE+="-DACOM_BOARD_TYPE=$ACOM_BOARD_TYPE "

# define the values of borad subtype
EXTRA_DEFINE+="-DRL00=0 -DRL01=1 -DRL02=2 -DRL03=3 -DMT_U3=10 -DMT_U4=11 -DMDM_01=20 -DMDM_02=21 -DMDM_03=22 -DMDM_04=23 -DMDM_05=24 -DMSM_01=31 -DSC_U501=41 "
EXTRA_DEFINE+="-DACOM_BOARD_SUBTYPE=$ACOM_BOARD_SUBTYPE "

# define the values of hardware type
EXTRA_DEFINE+="-DHWKS86954S=1 -DHWKS86958S=2 -DHWKS8695INFINEON2S=3 -DHWPPC4054S=4 -DHWPPC40516S=5 -DHWSAMSUNG4510B2S=6 -DHWPPC40532S=7 -DHWGOIP=8 -DHWVOIP=9 -DHWSIMPOOL=10 -DHWUUWIFI=11 "
EXTRA_DEFINE+="-DACOM_HW_TYPE=$ACOM_HW_TYPE -DACOM_PORT_NUM=$ACOM_PORT_NUM "

# define the values of sub-hw type
EXTRA_DEFINE+="-DHWST_NONE=0 -DHWST_3G4G=0 -DHWST_2G4G=1 "
EXTRA_DEFINE+="-DACOM_HW_SUBTYPE=$ACOM_HW_SUBTYPE "

# define the binary type of vad
EXTRA_DEFINE+="-DVAD_BIN_TYPE=\\\"$ACOM_BIN_TYPE\\\" "

INC_FLAGS="-I$SYSROOT/include -I$SYSROOT/usr/include "
INC_FLAGS+="-I$SYSROOT/usr/include/libril -I$SYSROOT/usr/include/qmi-framework -I$SYSROOT/usr/include/qmi "
INC_FLAGS+="-I$SYSROOT/usr/include/libmutils -I$SYSROOT/usr/include/libefs -I$SYSROOT/usr/include/libwifi "

###############################################################################################################
# define the arch environment
#
ARM_FLAGS="-march=armv7-a -mfloat-abi=softfp -mfpu=neon -fpic -DUSE_GLIB "
ARM_FLAGS+="$INC_FLAGS --sysroot=$SYSROOT $EXTRA_DEFINE"

CC=$TOOLSCHAIN_BIN/arm-oe-linux-gnueabi-gcc \
CXX=$TOOLSCHAIN_BIN/arm-oe-linux-gnueabi-g++ \
AS=$TOOLSCHAIN_BIN/arm-oe-linux-gnueabi-as \
LINK=$TOOLSCHAIN_BIN/arm-oe-linux-gnueabi-gcc \
LDFLAGS="-L$SYSROOT/lib -L$SYSROOT/usr/lib " \
./configure --enable-shared=yes $config_options --prefix=$VAD_PREFIX \
 --build=i686-pc-linux --host=arm-oe-linux-gnueabi --cache-file=arm-linux.cache \
 --with-board-type=$ACOM_BOARD_TYPE --with-hw-type=$ACOM_HW_TYPE --with-oem-name=$ACOM_OEM_NAME \
 CFLAGS="$ARM_FLAGS" \
 CXXFLAGS="$ARM_FLAGS -Wundef -Wno-trigraphs -fno-inline -fno-short-enums"
