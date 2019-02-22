#!/bin/bash

if [ -z "$1" ] ; then
  echo "usage: $0 open-wrt-home-dir"
  exit
fi

. ./config.sh RL-3G4G $1 $2
