#!/bin/bash

if [ -z "$1" ] ; then
  echo "usage: $0 mdm9x07-home-dir [vad_prefix]"
  exit
fi

BOARD_TYPE=${0%.*}
BOARD_TYPE="MDM9X07-${BOARD_TYPE##*-}"

. ./config-mdm9x07.sh "$BOARD_TYPE" "$@"
