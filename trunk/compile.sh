#!/bin/bash

export CXX=/usr/local/Cellar/gcc/7.3.0_1/bin/g++-7
export CC=/usr/local/Cellar/gcc/7.3.0_1/bin/gcc-7
./bootstrap.sh
make -Cbuild install