#/bin/sh
mkdir build
cd build 
rm -r *
rm .cmake
cmake -G "MinGW Makefiles" ..