#!/bin/bash

echo "usage: $0 [-s|--stdlib|--nossl|-t|--target]"

./mkall.sh "/opt/ejoin/tools/mdm9x07" "config-${0#*build-}" "" "rootfs_mdm9x07" "$@"
