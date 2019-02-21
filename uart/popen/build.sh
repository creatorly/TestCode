#!/bin/bash

./config.sh

make uninstall

make clean

make -j install
