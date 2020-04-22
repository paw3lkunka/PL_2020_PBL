#/bin/sh
mkdir build
cd build 
rm -r *
rm -r .cmake
cmake -G "MinGW Makefiles" ..