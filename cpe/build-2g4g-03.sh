#!/bin/bash

echo "usage: $0 [vad-prefix]"

./mkall.sh "/opt/ejoin/tools/openwrt-rl" "config-2g4g-03.sh" $1
