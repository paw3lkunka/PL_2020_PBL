#!/bin/sh
rm -rf build/Resources/*
cp -r resources/* build/Resources
if [[ $1 = "run" ]]; then
    ./run.sh ${@:2}
fi