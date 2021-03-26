#!/bin/sh -v
source ~/.bash_profile
rm -rf build
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr/local/bin/ ..
make -j10
make install
cd ../test
rm -rf build
mkdir build && cd build
cmake -DCMAKE_PREFIX_PATH:PATH=/usr/local/bin/ ..
make -j10
