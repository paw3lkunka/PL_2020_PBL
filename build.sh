#!/bin/sh

_file="$(dirname "$0")/misc/cpp_files_count"
touch $_file

_cpp1=$($(dirname "$0")/countcpp.sh)
_cpp2=$(< $_file)

if [[ $_cpp2 = "" ]]; then
    _cpp2=0
fi

if [[ $_cpp1 -ne $_cpp2 ]]; then
    echo "Number of cpp files was changed. Reconfiguring CMake." 
    ./config.sh
fi
cd build
make -j 12 CXXFLAGS="-g3 -Og"