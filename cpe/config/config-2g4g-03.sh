#!/bin/bash

if [ -z "$1" ] ; then
  echo "usage: $0 openwrt-home-dir [vad_prefix]"
  exit
fi

. ./config.sh RL-2G4G-03 $1 $2
