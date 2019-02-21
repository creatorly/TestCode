#!/bin/bash

chkDir() {
  prompt=${2:-"no such directory"}
  [ ! -d "$1" ] && echo -e "$1: $prompt" && exit
}

chkBin() {
  prompt=${2:-"no such binary file"}
  [ ! -x "$1" ] && echo -e "$1: $prompt" && exit
}

getFileDir() {
  local CUR_DIR=$PWD
  local filedir=$(dirname $1)

  cd $filedir
  filedir=$PWD

  cd $CUR_DIR

  echo $filedir
}

PKGTOOLS_DIR=$(getFileDir $0)
echo "$PKGTOOLS_DIR"
HOME=$(dirname $PKGTOOLS_DIR)
echo "$HOME"
chkDir "$HOME"
chkDir "$HOME/lib"
chkDir "$HOME/bin"

tar czvf $PKGTOOLS_DIR/bin.tgz lib/lib*.so*  bin/select
