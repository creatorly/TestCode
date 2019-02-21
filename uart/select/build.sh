#!/bin/bash

./config.sh

make uninstall
make distclean
make clean

make -j install
