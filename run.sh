#!/bin/sh

if [[ $1 = "cpr" ]]; then
    ./copyRes.sh
    cd build
    ./PBL* ${@:2}
else
    cd build
    ./PBL* $@
fi