#!/bin/sh -v
source ~/.bash_profile
rm -rf build
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr/local/bin/ ..
#cmake -DCMAKE_INSTALL_PREFIX:PATH=/home/r0829520/assign2/ ..
make -j10
make install
cd ../test
rm -rf build
mkdir build && cd build
cmake -DCMAKE_PREFIX_PATH:PATH=/usr/local/bin/ ..
#cmake -DCMAKE_PREFIX_PATH:PATH=/home/r0829520/assign2/ ..
make -j10
