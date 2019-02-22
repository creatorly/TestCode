#!/bin/bash

if [ -z "$1" ] ; then
  echo "usage: $0 mdm9x07-home-dir [vad_prefix]"
  exit
fi

EXTRA_DEFINE="-DVAD_FAC_CHK_SIGNAL=1"

. ./config-mdm9x07.sh "MDM9X07-01" "$1" "$2" "$EXTRA_DEFINE"
