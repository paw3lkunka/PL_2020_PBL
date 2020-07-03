#!/bin/sh

mkdir -p build/Resources 
rm -rf build/Resources/*
cp -r resources/* build/Resources
echo "Resources copied successfully!"

if [[ $1 = "run" ]]; then
    ./run.sh ${@:2}
fi