#!/bin/sh

mkdir -p build
cd build 
if [ "$1" = "clear" ]; then
    rm -r *
    rm -r .cmake
fi
cmake -G "MinGW Makefiles" ..