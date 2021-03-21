#!/bin/sh -v
source ~/.bash_profile
set -x
rm -rf build
mkdir build && cd build
cmake ..
make -j10