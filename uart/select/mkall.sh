#!/bin/bash

######################################################################################
# $1: tools_chain home directory
# $2: configure script
# $3: VAD prefix directory
# $4: rootfs relative directory(default is rootfs)
######################################################################################

getEnv() {
  env_val=`grep "$1"'=' .config.env`
  env_val=${env_val#*=}
  echo ${env_val:-$2}
}

progname=$0

ARGS=`getopt -o s,t:Q --long stdlib,nossl,disable-ssl,target:,quick -n "$progname" -- "$@"`
[ $? != 0 ] && exit

#echo $ARGS
#将规范化后的命令行参数分配至位置参数（$1,$2,...)
eval set -- "${ARGS}"

while true
do
  case "$1" in
    -s|--stdlib)
      stdlib=1
      shift
      ;;

    --nossl|--disable-ssl)
      config_options+=" --disable-ssl"
      shift
      ;;

    -t|--target)
      target=${2//,/ }
      shift 2
      ;;

    -Q|--quick)
      quick=1
      shift
      ;;
  
    --)
      shift
      break
      ;;

    *)
      echo "$progname: Parse arguments error!"
      exit 1
      ;;
  esac
done

echo -e "\nusage: $0 [-s|--stdlib|--nossl] \"CC_HOME\" \"config_sh\" \"vad_prefix\" \"rootfs_path\""

CC_HOME=$1
config_sh=$2
vad_prefix=$3
rootfs_dir=$4
extra_define=""

make_install() {

  if [ "$quick" != 1 ] ; then
    ./$config_sh "$CC_HOME" "$vad_prefix" "$extra_define" $config_options && {
      make uninstall
      make clean
    }
  fi

  make -j install && ret=0
}

make_install || exit

std_prefix=$(getEnv std_prefix)
echo "std_prefix:$std_prefix"
echo "stdlib:$stdlib"

if [ "$stdlib" = "1" ] ; then
  echo -e "\n###################################################"
  echo "Copy libraries to standard prefix $std_prefix ..."

  [ ! -d "$std_prefix" ] && mkdir -p $std_prefix
  /bin/cp -af include lib $std_prefix/ 
fi

pkgtools/loc_pkg.sh || exit
