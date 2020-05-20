#!/bin/sh
#cmake --build build --config Debug --target all -- -j 14
cd build
#ls -R project | grep "cpp" | wc -l
make -j 12
